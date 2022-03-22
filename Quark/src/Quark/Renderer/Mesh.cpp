#include "Mesh.h"
#include "Baked/Cube.h"

#include "../Core/Core.h"
#include "../Filesystem/Filesystem.h"

namespace Quark {

	struct Vertex
	{
		glm::vec3 Position;
		glm::vec2 TexCoord;
		glm::vec3 Normal;
	};

	static const Quark::BufferLayout s_Layout = {
		{ Quark::ShaderDataType::Float3, "a_Position" },
		{ Quark::ShaderDataType::Float2, "a_TexCoord" },
		{ Quark::ShaderDataType::Float3, "a_Normal"   }
	};

	Mesh::Mesh(std::string_view filepath)
	{
		LoadOBJFromFile(filepath);
	}

	void Mesh::GenerateUnitCube()
	{
		m_VertexArray = VertexArray::Create();

		auto vbo = VertexBuffer::Create(Cube::Vertices, sizeof(Cube::Vertices));
		vbo->SetLayout(s_Layout);

		m_VertexArray->AddVertexBuffer(vbo);

		auto ibo = IndexBuffer::Create(Cube::Indices, sizeof(Cube::Indices) / sizeof(uint32_t));
		m_VertexArray->SetIndexBuffer(ibo);
	}

	template<char Delim>
	static void Tokenize(std::string_view str, std::vector<std::string_view>& out)
	{
		size_t start;
		size_t end = 0;

		out.clear();
		while ((start = str.find_first_not_of(Delim, end)) != std::string::npos)
		{
			end = str.find(Delim, start);
			out.push_back(str.substr(start, end - start));
		}
	}

	static void SplitIndexData(std::string_view data, uint32_t* indices)
	{
		size_t begin = 0;
		uint32_t idx = 0;
		while (begin < data.size())
		{
			size_t end = data.find_first_of('/', begin);
			if (end == std::string::npos)
				end = data.size();

			if (end != begin)
			{
				uint32_t index;
				std::from_chars(data.data() + begin, data.data() + end, index);
				indices[idx] = index - 1; // -1 since .obj files are indexed starting at 1
			}
			else
			{
				indices[idx] = 0;
			}
			
			begin = end + 1;
			idx++;
		}
	}

	template<typename T>
	static void ExtractFromToken(std::string_view token, T& value)
	{
		std::from_chars(token.data(), token.data() + token.size(), value);
	}

	void Mesh::LoadOBJFromFile(std::string_view filepath)
	{
		QK_SCOPE_TIMER(Mesh::LoadOBJFromFile);

		const std::string fileRaw = Filesystem::ReadFile(filepath);

		std::string_view file = fileRaw;
		std::vector<std::string_view> tokens;

		std::vector<glm::vec3> positions;
		std::vector<glm::vec2> texCoords;
		std::vector<glm::vec3> normals;
		size_t faces = 0;

		size_t pos = 0;
		size_t eol;
		while ((eol = file.find_first_of("\r\n", pos)) != std::string::npos)
		{
			std::string_view line = file.substr(pos, eol - pos);
			pos = eol + 1;
			Tokenize<' '>(line, tokens);

			if (line.substr(0, 2) == "v " && tokens.size() >= 4)
			{
				glm::vec3 vertex;
				ExtractFromToken(tokens[1], vertex.x);
				ExtractFromToken(tokens[2], vertex.y);
				ExtractFromToken(tokens[3], vertex.z);
				positions.push_back(vertex);
			}
			else if (line.substr(0, 2) == "vt" && tokens.size() >= 3)
			{
				glm::vec2 texCoord;
				ExtractFromToken(tokens[1], texCoord.x);
				ExtractFromToken(tokens[2], texCoord.y);
				texCoords.push_back(texCoord);
			}
			else if (line.substr(0, 2) == "vn" && tokens.size() >= 4)
			{
				glm::vec3 normal;
				ExtractFromToken(tokens[1], normal.x);
				ExtractFromToken(tokens[2], normal.y);
				ExtractFromToken(tokens[3], normal.z);
				normals.push_back(normal);
			}
			else if (line.substr(0, 1) == "f" && tokens.size() >= 4)
			{
				faces++;
			}
			else if (line.substr(0, 1) == "s" && tokens.size() > 2)
			{
				if (tokens[1].contains("off") || tokens[1].contains('0'))
					m_SmoothShaded = false;
			}
		}

		if (faces == 0)
			return;

		if (texCoords.empty())
			texCoords.push_back(glm::vec2(0.0f));

		if (normals.empty())
			normals.push_back(glm::vec3(0.0f));

		Vertex* baseVertices = new Vertex[faces * 3];
		Vertex* vertexPtr = baseVertices;

		pos = 0;
		while ((eol = file.find_first_of("\r\n", pos)) != std::string::npos)
		{
			std::string_view line = file.substr(pos, eol - pos);
			pos = eol + 1;
			Tokenize<' '>(line, tokens);

			if (line.substr(0, 1) == "f" && tokens.size() >= 4)
			{
				uint32_t indices[3]; // pos_ptr // tex_ptr // norm_ptr
				for (size_t i = 0; i < 3; i++)
				{
					SplitIndexData(tokens[i + 1], indices);

					uint32_t v = indices[0];
					uint32_t t = indices[1];
					uint32_t n = indices[2];

					vertexPtr->Position = positions[v];
					vertexPtr->TexCoord = texCoords[t];
					vertexPtr->Normal   = normals[n];
					vertexPtr++;
				}
			}
		}

		uint32_t* indices = new uint32_t[faces * 3];
		for (uint32_t i = 0; i < faces * 3; i++)
		{
			indices[i] = i;
		}

		m_VertexArray = VertexArray::Create();

		auto vbo = VertexBuffer::Create(baseVertices, faces * 3 * sizeof(Vertex));
		vbo->SetLayout(s_Layout);
		m_VertexArray->AddVertexBuffer(vbo);

		auto ibo = IndexBuffer::Create(indices, faces * 3);
		m_VertexArray->SetIndexBuffer(ibo);

		delete[] baseVertices;
		delete[] indices;
	}
}
