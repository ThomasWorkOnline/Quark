#include "Mesh.h"
#include "Baked/Cube.h"

#include "Quark/Filesystem/Filesystem.h"

#include <charconv>
#include <string_view>
#include <vector>

namespace Quark {

	struct Vertex
	{
		glm::vec3 Position;
		glm::vec2 TexCoord;
		glm::vec3 Normal;
	};

	struct IndexPack
	{
		uint32_t PositionIndex;
		uint32_t TexCoordIndex;
		uint32_t NormalIndex;

		IndexPack(uint32_t pi, uint32_t ti, uint32_t ni)
			: PositionIndex(pi), TexCoordIndex(ti), NormalIndex(ni)
		{
		}
	};

	static const Quark::BufferLayout s_Layout = {
		{ Quark::ShaderDataType::Float3, "a_Position" },
		{ Quark::ShaderDataType::Float2, "a_TexCoord" },
		{ Quark::ShaderDataType::Float3, "a_Normal"   }
	};

	struct OBJData
	{
		std::vector<glm::vec3> Positions;
		std::vector<glm::vec2> TexCoords;
		std::vector<glm::vec3> Normals;
		std::vector<IndexPack> FacesIndices;
		bool SmoothShaded = true;

		size_t VertexCount() const { return FacesIndices.size(); }
	};

	static void Tokenize(std::string_view str, std::vector<std::string_view>& out)
	{
		size_t start;
		size_t end = 0;

		out.clear();
		while ((start = str.find_first_not_of(' ', end)) != std::string::npos)
		{
			end = str.find(' ', start);
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

	static float ExtractFromToken(std::string_view token)
	{
		char* end;
		return std::strtof(token.data(), &end);
	}

	static void ReadOBJData(std::string_view filepath, OBJData& data)
	{
		const std::string fileRaw = Filesystem::ReadFile(filepath);

		std::string_view file = fileRaw;
		std::vector<std::string_view> tokens;

		size_t pos = 0;
		size_t eol;
		while ((eol = file.find_first_of("\r\n", pos)) != std::string::npos)
		{
			std::string_view line = file.substr(pos, eol - pos);
			pos = eol + 1;
			Tokenize(line, tokens);

			if (line.substr(0, 2) == "v " && tokens.size() >= 4)
			{
				glm::vec3 vertex;
				vertex.x = ExtractFromToken(tokens[1]);
				vertex.y = ExtractFromToken(tokens[2]);
				vertex.z = ExtractFromToken(tokens[3]);
				data.Positions.push_back(vertex);
			}
			else if (line.substr(0, 2) == "vt" && tokens.size() >= 3)
			{
				glm::vec2 texCoord;
				texCoord.x = ExtractFromToken(tokens[1]);
				texCoord.y = ExtractFromToken(tokens[2]);
				data.TexCoords.push_back(texCoord);
			}
			else if (line.substr(0, 2) == "vn" && tokens.size() >= 4)
			{
				glm::vec3 normal;
				normal.x = ExtractFromToken(tokens[1]);
				normal.y = ExtractFromToken(tokens[2]);
				normal.z = ExtractFromToken(tokens[3]);
				data.Normals.push_back(normal);
			}
			else if (line.substr(0, 1) == "f" && tokens.size() >= 4)
			{
				uint32_t indices[3]; // pos_ptr // tex_ptr // norm_ptr
				for (size_t i = 0; i < 3; i++)
				{
					SplitIndexData(tokens[i + 1], indices);

					uint32_t v = indices[0];
					uint32_t t = indices[1];
					uint32_t n = indices[2];

					data.FacesIndices.emplace_back(v, t, n);
				}
			}
			else if (line.substr(0, 1) == "s" && tokens.size() >= 2)
			{
				if (tokens[1].find("off") != std::string::npos || tokens[1].find('0') != std::string::npos)
					data.SmoothShaded = false;
			}
		}

		if (data.Positions.empty())
			data.Positions.push_back(glm::vec3(0.0f));

		if (data.TexCoords.empty())
			data.TexCoords.push_back(glm::vec2(0.0f));

		if (data.Normals.empty())
			data.Normals.push_back(glm::vec3(0.0f));
	}

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

	void Mesh::LoadOBJFromFile(std::string_view filepath)
	{
		QK_SCOPE_TIMER(Mesh::LoadOBJFromFile);

		OBJData data;
		ReadOBJData(filepath, data);

		size_t vertexCount = data.VertexCount();
		if (vertexCount == 0)
			return;

		Vertex* baseVertices = new Vertex[vertexCount];
		Vertex* vertexPtr = baseVertices;

		for (auto& idx : data.FacesIndices)
		{
			vertexPtr->Position = data.Positions[idx.PositionIndex];
			vertexPtr->TexCoord = data.TexCoords[idx.TexCoordIndex];
			vertexPtr->Normal   = data.Normals[idx.NormalIndex];
			vertexPtr++;
		}

		uint32_t* indices = new uint32_t[vertexCount];
		for (uint32_t i = 0; i < vertexCount; i++)
		{
			indices[i] = i;
		}

		m_VertexArray = VertexArray::Create();

		auto vbo = VertexBuffer::Create(baseVertices, vertexCount * sizeof(Vertex));
		vbo->SetLayout(s_Layout);
		m_VertexArray->AddVertexBuffer(vbo);

		auto ibo = IndexBuffer::Create(indices, vertexCount);
		m_VertexArray->SetIndexBuffer(ibo);

		delete[] baseVertices;
		delete[] indices;
	}
}
