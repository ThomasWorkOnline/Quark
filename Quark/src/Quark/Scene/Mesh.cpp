#include "qkpch.h"
#include "Mesh.h"

#include "Quark/Filesystem/Filesystem.h"

#include <charconv>
#include <vector>

namespace Quark {

	static constexpr float CubeVertices[] = {
		// front
		0.5f, -0.5f, -0.5f,   0.0f, 1.0f,   0.0f,  0.0f, -1.0f,
	   -0.5f, -0.5f, -0.5f,   1.0f, 1.0f,   0.0f,  0.0f, -1.0f,
	   -0.5f,  0.5f, -0.5f,   1.0f, 0.0f,   0.0f,  0.0f, -1.0f,
		0.5f,  0.5f, -0.5f,   0.0f, 0.0f,   0.0f,  0.0f, -1.0f,

		// back
	   -0.5f, -0.5f,  0.5f,   0.0f, 1.0f,   0.0f,  0.0f,  1.0f,
		0.5f, -0.5f,  0.5f,   1.0f, 1.0f,   0.0f,  0.0f,  1.0f,
		0.5f,  0.5f,  0.5f,   1.0f, 0.0f,   0.0f,  0.0f,  1.0f,
	   -0.5f,  0.5f,  0.5f,   0.0f, 0.0f,   0.0f,  0.0f,  1.0f,

	    // right						   
	    0.5f, -0.5f,  0.5f,   0.0f, 1.0f,   1.0f,  0.0f,  0.0f,
	    0.5f, -0.5f, -0.5f,   1.0f, 1.0f,   1.0f,  0.0f,  0.0f,
	    0.5f,  0.5f, -0.5f,   1.0f, 0.0f,   1.0f,  0.0f,  0.0f,
	    0.5f,  0.5f,  0.5f,   0.0f, 0.0f,   1.0f,  0.0f,  0.0f,

	    // left
	   -0.5f, -0.5f, -0.5f,   0.0f, 1.0f,  -1.0f,  0.0f,  0.0f,
	   -0.5f, -0.5f,  0.5f,   1.0f, 1.0f,  -1.0f,  0.0f,  0.0f,
	   -0.5f,  0.5f,  0.5f,   1.0f, 0.0f,  -1.0f,  0.0f,  0.0f,
	   -0.5f,  0.5f, -0.5f,   0.0f, 0.0f,  -1.0f,  0.0f,  0.0f,

	    // bottom										  
	   -0.5f, -0.5f, -0.5f,   0.0f, 1.0f,   0.0f, -1.0f,  0.0f,
	    0.5f, -0.5f, -0.5f,   1.0f, 1.0f,   0.0f, -1.0f,  0.0f,
	    0.5f, -0.5f,  0.5f,   1.0f, 0.0f,   0.0f, -1.0f,  0.0f,
	   -0.5f, -0.5f,  0.5f,   0.0f, 0.0f,   0.0f, -1.0f,  0.0f,

	    // top											  
	   -0.5f,  0.5f,  0.5f,   0.0f, 1.0f,   0.0f,  1.0f,  0.0f,
	    0.5f,  0.5f,  0.5f,   1.0f, 1.0f,   0.0f,  1.0f,  0.0f,
	    0.5f,  0.5f, -0.5f,   1.0f, 0.0f,   0.0f,  1.0f,  0.0f,
	   -0.5f,  0.5f, -0.5f,   0.0f, 0.0f,   0.0f,  1.0f,  0.0f
	};

	static constexpr uint32_t CubeIndices[] = {
		// front
		0, 1, 2,
		2, 3, 0,

		// right
		4, 5, 6,
		6, 7, 4,

		// back
		8, 9, 10,
		10, 11, 8,

		// left
		12, 13, 14,
		14, 15, 12,

		// bottom
		16, 17, 18,
		18, 19, 16,

		// top
		20, 21, 22,
		22, 23, 20
	};

	static const BufferLayout s_Layout = {
		{ ShaderDataType::Float3, "a_Position" },
		{ ShaderDataType::Float2, "a_TexCoord" },
		{ ShaderDataType::Float3, "a_Normal"   }
	};

	Mesh::Mesh(const OBJMeshData& meshData)
		: Mesh(ConstructMeshFromOBJData(meshData))
	{
	}

	// TODO: rework all this mess
	Mesh Mesh::ConstructMeshFromOBJData(const OBJMeshData& data)
	{
		QK_PROFILE_FUNCTION();

		uint32_t vertexCount = data.VertexCount();
		if (vertexCount == 0 || vertexCount % 3 != 0)
		{
			QK_CORE_ERROR("Could not triangulate faces: {0} vertices received", vertexCount);
			return {};
		}

		MeshVertex* baseVertices = new MeshVertex[vertexCount];
		MeshVertex* vertexPtr = baseVertices;

		for (auto& idx : data.FacesIndices)
		{
			vertexPtr->Position = data.Positions[idx.PositionIndex];
			vertexPtr->TexCoord = data.TexCoords[idx.TexCoordIndex];
			vertexPtr->Normal   = data.Normals[idx.NormalIndex];
			vertexPtr++;
		}

		// TODO: fix (indexing here is useless)
		uint32_t* indices = new uint32_t[vertexCount];
		for (uint32_t i = 0; i < vertexCount; i += 3)
		{
			indices[i + 0] = i + 0;
			indices[i + 1] = i + 2;
			indices[i + 2] = i + 1;
		}

		Mesh mesh;
		mesh.m_VertexBuffer = VertexBuffer::Create(baseVertices, vertexCount * sizeof(MeshVertex));
		mesh.m_VertexBuffer->SetLayout(s_Layout);

		mesh.m_IndexBuffer = IndexBuffer::Create(indices, vertexCount);

		delete[] baseVertices;
		delete[] indices;

		return mesh;
	}

	Mesh Mesh::GenerateUnitCube()
	{
		QK_PROFILE_FUNCTION();

		Mesh mesh;
		mesh.m_VertexBuffer = VertexBuffer::Create(CubeVertices, sizeof(CubeVertices));
		mesh.m_VertexBuffer->SetLayout(s_Layout);

		mesh.m_IndexBuffer = IndexBuffer::Create(CubeIndices, sizeof_array(CubeIndices));
		return mesh;
	}

	// Reading and parsing files
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

	const BufferLayout& Mesh::GetBufferLayout()
	{
		return s_Layout;
	}

	OBJMeshData Mesh::ReadOBJData(std::string_view filepath)
	{
		QK_PROFILE_FUNCTION();

#if 1
		const std::string fileRaw = Filesystem::ReadTextFile(filepath);

		OBJMeshData data;
		std::string_view file = fileRaw;
		std::vector<std::string_view> tokens(4);

		size_t pos = 0;
		size_t eol;
		while ((eol = file.find_first_of("\r\n", pos)) != std::string::npos)
		{
			std::string_view line = file.substr(pos, eol - pos);
			pos = eol + 1;
			Tokenize(line, tokens);

			if (line.substr(0, 2) == "v " && tokens.size() >= 4)
			{
				data.Positions.emplace_back(
					ExtractFromToken(tokens[1]),
					ExtractFromToken(tokens[2]),
					ExtractFromToken(tokens[3])
				);
			}
			else if (line.substr(0, 2) == "vt" && tokens.size() >= 3)
			{
				data.TexCoords.emplace_back(
					ExtractFromToken(tokens[1]),
					ExtractFromToken(tokens[2])
				);
			}
			else if (line.substr(0, 2) == "vn" && tokens.size() >= 4)
			{
				data.Normals.emplace_back(
					ExtractFromToken(tokens[1]),
					ExtractFromToken(tokens[2]),
					ExtractFromToken(tokens[3])
				);
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
		{
			data.Positions.emplace_back(0.0f);
			QK_CORE_WARN("Invalid mesh: does not contain position information.");
		}

		if (data.TexCoords.empty())
		{
			data.TexCoords.emplace_back(0.0f);
			QK_CORE_WARN("Invalid mesh: does not contain texCoord information.");
		}

		if (data.Normals.empty())
		{
			data.Normals.emplace_back(0.0f);
			QK_CORE_WARN("Invalid mesh: does not contain normal information.");
		}

		return data;
#endif
	}
}
