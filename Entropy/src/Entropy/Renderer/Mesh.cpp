#include "Mesh.h"

#include "../Tools/Noise.h"

#include <fstream>

namespace Entropy {

	Mesh::Mesh(const BufferLayout& layout, const char* filepath)
	{
		NT_ASSERT(LoadOBJFromFile(layout, filepath), "Could not load model at path : " << filepath);
	}

	void Mesh::GenerateUnitCube(const BufferLayout& layout)
	{
		m_VertexArray = VertexArray::Create();

		float vertexBuffer[] = {
			// front
		   -0.5f, -0.5f,  0.5f,   0.0f, 0.0f,   0.0f,  0.0f,  1.0f,
			0.5f, -0.5f,  0.5f,   1.0f, 0.0f,   0.0f,  0.0f,  1.0f,
			0.5f,  0.5f,  0.5f,   1.0f, 1.0f,   0.0f,  0.0f,  1.0f,
		   -0.5f,  0.5f,  0.5f,   0.0f, 1.0f,   0.0f,  0.0f,  1.0f,

		    // right
		    0.5f, -0.5f,  0.5f,   0.0f, 0.0f,   1.0f,  0.0f,  0.0f,
		    0.5f, -0.5f, -0.5f,   1.0f, 0.0f,   1.0f,  0.0f,  0.0f,
		    0.5f,  0.5f, -0.5f,   1.0f, 1.0f,   1.0f,  0.0f,  0.0f,
		    0.5f,  0.5f,  0.5f,   0.0f, 1.0f,   1.0f,  0.0f,  0.0f,

		    // back
		    0.5f, -0.5f, -0.5f,   0.0f, 0.0f,   0.0f,  0.0f, -1.0f,
		   -0.5f, -0.5f, -0.5f,   1.0f, 0.0f,   0.0f,  0.0f, -1.0f,
		   -0.5f,  0.5f, -0.5f,   1.0f, 1.0f,   0.0f,  0.0f, -1.0f,
		    0.5f,  0.5f, -0.5f,   0.0f, 1.0f,   0.0f,  0.0f, -1.0f,

		    // left
		   -0.5f, -0.5f, -0.5f,   0.0f, 0.0f,  -1.0f,  0.0f,  0.0f,
		   -0.5f, -0.5f,  0.5f,   1.0f, 0.0f,  -1.0f,  0.0f,  0.0f,
		   -0.5f,  0.5f,  0.5f,   1.0f, 1.0f,  -1.0f,  0.0f,  0.0f,
		   -0.5f,  0.5f, -0.5f,   0.0f, 1.0f,  -1.0f,  0.0f,  0.0f,

		    // bottom
		   -0.5f, -0.5f, -0.5f,   0.0f, 0.0f,   0.0f, -1.0f,  0.0f,
		    0.5f, -0.5f, -0.5f,   1.0f, 0.0f,   0.0f, -1.0f,  0.0f,
		    0.5f, -0.5f,  0.5f,   1.0f, 1.0f,   0.0f, -1.0f,  0.0f,
		   -0.5f, -0.5f,  0.5f,   0.0f, 1.0f,   0.0f, -1.0f,  0.0f,

		    // top
		   -0.5f,  0.5f,  0.5f,   0.0f, 0.0f,   0.0f,  1.0f,  0.0f,
		    0.5f,  0.5f,  0.5f,   1.0f, 0.0f,   0.0f,  1.0f,  0.0f,
		    0.5f,  0.5f, -0.5f,   1.0f, 1.0f,   0.0f,  1.0f,  0.0f,
		   -0.5f,  0.5f, -0.5f,   0.0f, 1.0f,   0.0f,  1.0f,  0.0f
		};

		uint32_t indexBuffer[] = {
			// front
			0, 2, 3,
			1, 2, 0,

			// right
			4, 6, 7,
			5, 6, 4,

			// back
			8, 10, 11,
			9, 10, 8,

			// left
			12, 14, 15,
			13, 14, 12,

			// bottom
			16, 18, 19,
			17, 18, 16,

			// top
			20, 22, 23,
			21, 22, 20
		};

		// Takes size in bytes
		m_VertexBuffer = VertexBuffer::Create(vertexBuffer, sizeof(vertexBuffer));
		m_VertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(m_VertexBuffer);

		// Takes index count
		m_IndexBuffer = IndexBuffer::Create(indexBuffer, sizeof(indexBuffer) / sizeof(uint32_t));
		m_VertexArray->SetIndexBuffer(m_IndexBuffer);
	}

	void Mesh::GenerateTerrain(const BufferLayout& layout, size_t scale, uint32_t seed)
	{
		NT_ASSERT(scale > 0, "Terrain scale cannot be smaller than 1");

		m_VertexArray = VertexArray::Create();

		size_t size = scale + 1;
		size_t strideCount = layout.GetStride() / sizeof(float);
		size_t vertexBufferCount = size * size * strideCount;
		float* vertexBuffer = new float[vertexBufferCount];

		std::vector<uint32_t> indexBuffer;

		// To sample the height of the terrain, we need to know the height of the border
		size_t noiseSize = size + 1;
		float* noiseSeed2D = new float[noiseSize * noiseSize];
		for (int i = 0; i < noiseSize * noiseSize; i++)
		{
			float value = (float)rand() / (float)RAND_MAX;
			noiseSeed2D[i] = value * 0.2f;
		}
		//float* perlinNoise2D = new float[noiseSize * noiseSize];
		//PerlinNoise2D(noiseSize, noiseSize, noiseSeed2D, 10, 2.4f, perlinNoise2D);*/

		for (size_t y = 0; y < size; y++)
		{
			for (size_t x = 0; x < size; x++)
			{
				size_t pos = strideCount * (y * size + x);

				static const float centerOffset = scale * 0.5f;

				float height0 = noiseSeed2D[(x + 0) + (y + 0) * noiseSize]; // center;
				float height1 = noiseSeed2D[(x + 1) + (y + 0) * noiseSize]; // right
				float height2 = noiseSeed2D[(x + 0) + (y + 1) * noiseSize]; // bottom

				// Position
				vertexBuffer[pos + 0] = (float)x - centerOffset;
				vertexBuffer[pos + 1] = (float)height0;
				vertexBuffer[pos + 2] = (float)y - centerOffset;

				// TexCoord
				vertexBuffer[pos + 3] = (float)x;
				vertexBuffer[pos + 4] = (float)y;

				// Normal
				glm::vec3 line1(glm::vec3(x, 0.0f, y + 1) - glm::vec3(x, 0.0f, y)); // bottom
				glm::vec3 line2(glm::vec3(x + 1, 0.0f, y) - glm::vec3(x, 0.0f, y)); // right
				glm::vec3 normal = glm::normalize(glm::cross(line1, line2));

				vertexBuffer[pos + 5] = normal.x;
				vertexBuffer[pos + 6] = normal.y;
				vertexBuffer[pos + 7] = normal.z;
			}
		}

		delete[] noiseSeed2D;
		//delete[] perlinNoise2D;

		for (size_t y = 0; y < scale; y++)
		{
			for (size_t x = 0; x < scale; x++)
			{
				// face #1
				indexBuffer.push_back((y + 0) * size + (x + 0));
				indexBuffer.push_back((y + 1) * size + (x + 0));
				indexBuffer.push_back((y + 0) * size + (x + 1));

				// face #2
				indexBuffer.push_back((y + 1) * size + (x + 1));
				indexBuffer.push_back((y + 0) * size + (x + 1));
				indexBuffer.push_back((y + 1) * size + (x + 0));
			}
		}

		m_VertexBuffer = VertexBuffer::Create(vertexBuffer, vertexBufferCount * sizeof(float));
		m_VertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(m_VertexBuffer);

		m_IndexBuffer = IndexBuffer::Create(&indexBuffer[0], (uint32_t)indexBuffer.size());
		m_VertexArray->SetIndexBuffer(m_IndexBuffer);

		delete[] vertexBuffer;
	}

	void Tokenize(const std::string& str, const char delim, std::vector<std::string>& out)
	{
		size_t start;
		size_t end = 0;

		out.clear();
		while ((start = str.find_first_not_of(delim, end)) != std::string::npos)
		{
			end = str.find(delim, start);
			out.push_back(str.substr(start, end - start));
		}
	}

	void SplitIndexData(const std::string& data, uint32_t indices[])
	{
		std::string sIndex = "";
		int32_t count = 0;
		for (uint32_t i = 0; i <= data.length(); i++)
		{
			if (data[i] != '/')
			{
				// Char is numerical
				sIndex += data[i];
			}
			else if (data[i] == '/')
			{
				if (sIndex.length() != 0)
					indices[count] = std::stoul(sIndex);
				else
					indices[count] = 0;

				count++;
				sIndex = "";
			}
		}
		indices[count] = std::stoul(sIndex);
	}

	bool Mesh::LoadOBJFromFile(const BufferLayout& layout, const char* filepath)
	{
		std::ios::sync_with_stdio(false);

		std::ifstream in(filepath, std::ios::in);
		if (!in.is_open())
			return false;

		std::vector<glm::vec3> positions;
		std::vector<glm::vec2> textures;
		std::vector<glm::vec3> normals;

		std::string line;
		std::vector<std::string> tokens;

		// by default, smooth shading should always be true
		bool smoothShaded = true;

		// PHASE 1 - LOADING SECTION -------------------------------------------
		while (true)
		{
			std::getline(in, line);
			Tokenize(line, ' ', tokens);

			if (line.substr(0, 2) == "v ")
			{
				glm::vec3 vertex = {};
				vertex.x = std::stof(tokens[1]);
				vertex.y = std::stof(tokens[2]);
				vertex.z = std::stof(tokens[3]);
				positions.push_back(vertex);
			}
			else if (line.substr(0, 2) == "vt")
			{
				glm::vec2 texCoord = {};
				texCoord.x = std::stof(tokens[1]);
				texCoord.y = std::stof(tokens[2]);
				textures.push_back(texCoord);
			}
			else if (line.substr(0, 2) == "vn")
			{
				glm::vec3 normal = {};
				normal.x = std::stof(tokens[1]);
				normal.y = std::stof(tokens[2]);
				normal.z = std::stof(tokens[3]);
				normals.push_back(normal);
			}
			else if (line.substr(0, 2) == "s ")
			{
				if (line.substr(2, 3) == "off")
				{
					smoothShaded = false;
					NT_TRACE("Model is flat shaded");
				}
			}
			else if (line.substr(0, 2) == "f ")
				break;
		}

		m_VertexArray = VertexArray::Create();

		// PHASE 2 - COMPUTING SECTION -------------------------------------------
		uint32_t strideCount = (uint32_t)layout.GetStride() / sizeof(float);
		uint32_t vertexBufferCount = (uint32_t)positions.size() * strideCount;
		float* vertexBuffer = new float[vertexBufferCount];
		std::vector<uint32_t> indexBuffer;

		while (!in.eof())
		{
			if (line.substr(0, 2) == "f ")
			{
				uint32_t vertex0[3]; // pos_ptr // tex_ptr // norm_ptr
				uint32_t vertex1[3];
				uint32_t vertex2[3];

				// Flipping winding order for loading
#if NT_FLIP_ON_LOAD
#	define NT_FLIP_SIGN -
				SplitIndexData(tokens[1], vertex0);
				SplitIndexData(tokens[3], vertex1);
				SplitIndexData(tokens[2], vertex2);
#else
#	define NT_FLIP_SIGN
				SplitIndexData(tokens[1], vertex0);
				SplitIndexData(tokens[2], vertex1);
				SplitIndexData(tokens[3], vertex2);
#endif

				uint32_t v0 = vertex0[0] - 1; uint32_t v1 = vertex1[0] - 1; uint32_t v2 = vertex2[0] - 1;
				uint32_t t0 = vertex0[1] - 1; uint32_t t1 = vertex1[1] - 1; uint32_t t2 = vertex2[1] - 1;
				uint32_t n0 = vertex0[2] - 1; uint32_t n1 = vertex1[2] - 1; uint32_t n2 = vertex2[2] - 1;

				if (textures.empty())
				{
					vertexBuffer[strideCount * v0 + 0] = positions[v0].x;
					vertexBuffer[strideCount * v0 + 1] = positions[v0].y;
					vertexBuffer[strideCount * v0 + 2] = NT_FLIP_SIGN positions[v0].z;
					vertexBuffer[strideCount * v0 + 3] = 0.0f;
					vertexBuffer[strideCount * v0 + 4] = 0.0f;
					vertexBuffer[strideCount * v0 + 5] = normals[n0].x;
					vertexBuffer[strideCount * v0 + 6] = normals[n0].y;
					vertexBuffer[strideCount * v0 + 7] = NT_FLIP_SIGN normals[n0].z;

					vertexBuffer[strideCount * v1 + 0] = positions[v1].x;
					vertexBuffer[strideCount * v1 + 1] = positions[v1].y;
					vertexBuffer[strideCount * v1 + 2] = NT_FLIP_SIGN positions[v1].z;
					vertexBuffer[strideCount * v1 + 3] = 0.0f;
					vertexBuffer[strideCount * v1 + 4] = 0.0f;
					vertexBuffer[strideCount * v1 + 5] = normals[n1].x;
					vertexBuffer[strideCount * v1 + 6] = normals[n1].y;
					vertexBuffer[strideCount * v1 + 7] = NT_FLIP_SIGN normals[n1].z;

					vertexBuffer[strideCount * v2 + 0] = positions[v2].x;
					vertexBuffer[strideCount * v2 + 1] = positions[v2].y;
					vertexBuffer[strideCount * v2 + 2] = NT_FLIP_SIGN positions[v2].z;
					vertexBuffer[strideCount * v2 + 3] = 0.0f;
					vertexBuffer[strideCount * v2 + 4] = 0.0f;
					vertexBuffer[strideCount * v2 + 5] = normals[n2].x;
					vertexBuffer[strideCount * v2 + 6] = normals[n2].y;
					vertexBuffer[strideCount * v2 + 7] = NT_FLIP_SIGN normals[n2].z;
				}
				else
				{
					vertexBuffer[strideCount * v0 + 0] = positions[v0].x;
					vertexBuffer[strideCount * v0 + 1] = positions[v0].y;
					vertexBuffer[strideCount * v0 + 2] = NT_FLIP_SIGN positions[v0].z;
					vertexBuffer[strideCount * v0 + 3] = textures[t0].x;
					vertexBuffer[strideCount * v0 + 4] = textures[t0].y;
					vertexBuffer[strideCount * v0 + 5] = normals[n0].x;
					vertexBuffer[strideCount * v0 + 6] = normals[n0].y;
					vertexBuffer[strideCount * v0 + 7] = NT_FLIP_SIGN normals[n0].z;

					vertexBuffer[strideCount * v1 + 0] = positions[v1].x;
					vertexBuffer[strideCount * v1 + 1] = positions[v1].y;
					vertexBuffer[strideCount * v1 + 2] = NT_FLIP_SIGN positions[v1].z;
					vertexBuffer[strideCount * v1 + 3] = textures[t1].x;
					vertexBuffer[strideCount * v1 + 4] = textures[t1].y;
					vertexBuffer[strideCount * v1 + 5] = normals[n1].x;
					vertexBuffer[strideCount * v1 + 6] = normals[n1].y;
					vertexBuffer[strideCount * v1 + 7] = NT_FLIP_SIGN normals[n1].z;

					vertexBuffer[strideCount * v2 + 0] = positions[v2].x;
					vertexBuffer[strideCount * v2 + 1] = positions[v2].y;
					vertexBuffer[strideCount * v2 + 2] = NT_FLIP_SIGN positions[v2].z;
					vertexBuffer[strideCount * v2 + 3] = textures[t2].x;
					vertexBuffer[strideCount * v2 + 4] = textures[t2].y;
					vertexBuffer[strideCount * v2 + 5] = normals[n2].x;
					vertexBuffer[strideCount * v2 + 6] = normals[n2].y;
					vertexBuffer[strideCount * v2 + 7] = NT_FLIP_SIGN normals[n2].z;
				}

				indexBuffer.push_back(v0);
				indexBuffer.push_back(v1);
				indexBuffer.push_back(v2);
			}

			std::getline(in, line);
			Tokenize(line, ' ', tokens);
		}

		in.close();

		m_VertexBuffer = VertexBuffer::Create(vertexBuffer, vertexBufferCount * sizeof(float));
		m_VertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(m_VertexBuffer);

		m_IndexBuffer = IndexBuffer::Create(&indexBuffer[0], (uint32_t)indexBuffer.size());
		m_VertexArray->SetIndexBuffer(m_IndexBuffer);

		delete[] vertexBuffer;

		return true;
	}
}
