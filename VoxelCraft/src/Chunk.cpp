#include "Chunk.h"

#include <fstream>

#include "ConvertPosition.h"
#include "World.h"
#include "Resources.h"

static constexpr glm::vec3 s_VertexPositions[24] = {
	// front
	{  0.0f,  0.0f,  1.0f },
	{  1.0f,  0.0f,  1.0f },
	{  1.0f,  1.0f,  1.0f },
	{  0.0f,  1.0f,  1.0f },

	// right
	{  1.0f,  0.0f,  1.0f },
	{  1.0f,  0.0f,  0.0f },
	{  1.0f,  1.0f,  0.0f },
	{  1.0f,  1.0f,  1.0f },

	// back
	{  1.0f,  0.0f,  0.0f },
	{  0.0f,  0.0f,  0.0f },
	{  0.0f,  1.0f,  0.0f },
	{  1.0f,  1.0f,  0.0f },

	// left
	{  0.0f,  0.0f,  0.0f },
	{  0.0f,  0.0f,  1.0f },
	{  0.0f,  1.0f,  1.0f },
	{  0.0f,  1.0f,  0.0f },

	// top
	{  0.0f,  1.0f,  1.0f },
	{  1.0f,  1.0f,  1.0f },
	{  1.0f,  1.0f,  0.0f },
	{  0.0f,  1.0f,  0.0f },

	// bottom
	{  1.0f,  0.0f,  1.0f },
	{  0.0f,  0.0f,  1.0f },
	{  0.0f,  0.0f,  0.0f },
	{  1.0f,  0.0f,  0.0f }
};

static int32_t IndexAtPosition(const glm::ivec3& position)
{
	if (position.x < 0 || position.y < 0 || position.z < 0
		|| position.x >= ChunkSpecification::Width || position.y >= ChunkSpecification::Height || position.z >= ChunkSpecification::Depth)
		return -1;
	return position.x + ChunkSpecification::Width * position.z + ChunkSpecification::Width * ChunkSpecification::Depth * position.y;
}

static const Quark::BufferLayout s_BufferLayout = {
	{ Quark::ShaderDataType::Float3, "a_Position"  },
	{ Quark::ShaderDataType::Float2, "a_TexCoord"  },
	{ Quark::ShaderDataType::Float,  "a_Intensity" }
};

Chunk::Chunk(size_t id, World* world)
	: m_Id(id), m_World(world) {}

Chunk::~Chunk()
{
	delete[] m_Blocks;
	delete[] m_HeightMap;
}

static void DumpAsBinary(std::ofstream& out, const void* data, size_t size)
{
	out.write(reinterpret_cast<const char*>(data), size);
}

void Chunk::Save() const
{
	/*if (this)
	{
		std::stringstream filepath;
		filepath << CHUNK_UUID(m_Coord);
		filepath << ".dat";

		std::ofstream out(filepath.str(), std::ios::out | std::ios::binary);

		DumpAsBinary(out, this, sizeof(Chunk));

		out.close();
	}*/
}

void Chunk::BuildTerrain()
{
	QK_ASSERT(m_Blocks == nullptr, "Tried to allocate blocks more than once");

	// Generate height map
	m_HeightMap = new int32_t[ChunkSpecification::Width * ChunkSpecification::Depth];

	for (uint32_t z = 0; z < ChunkSpecification::Depth; z++)
	{
		for (uint32_t x = 0; x < ChunkSpecification::Width; x++)
		{
			Position2D pos(x, z);
			auto position = pos.ToWorldSpace(m_Coord);

			// Important to use deterministic algorithms for anything related to world gen
			float fValue = round(sinf(position.x * 0.144f + position.y * 0.021f) * 6.0f);
			int32_t value = static_cast<int32_t>(fValue);
			m_HeightMap[z * ChunkSpecification::Depth + x] = value;
		}
	}

	// Generate blocks
	m_Blocks = new Block[ChunkSpecification::BlockCount];

	for (uint32_t y = 0; y < ChunkSpecification::Height; y++)
	{
		for (uint32_t z = 0; z < ChunkSpecification::Depth; z++)
		{
			for (uint32_t x = 0; x < ChunkSpecification::Width; x++)
			{
				m_Blocks[IndexAtPosition({ x, y, z })] = GenerateBlock({ x, y, z });
			}
		}
	}
}

void Chunk::BuildMesh(Chunk* left, Chunk* right, Chunk* back, Chunk* front)
{
	m_VertexCount = 0;
	m_IndexCount = 0;
	m_Vertices.clear();
	m_Indices.clear();

	QK_ASSERT(left, "");
	QK_ASSERT(right, "");
	QK_ASSERT(back, "");
	QK_ASSERT(front, "");

	for (uint32_t y = 0; y < ChunkSpecification::Height; y++)
	{
		for (uint32_t z = 0; z < ChunkSpecification::Depth; z++)
		{
			for (uint32_t x = 0; x < ChunkSpecification::Width; x++)
			{
				Block block = GetBlock({ x, y, z });

				if (block == Block::Air)
					continue;

				for (uint8_t f = 0; f < 6; f++)
				{
					if (!IsBlockFaceVisible({ x, y, z }, static_cast<BlockFace>(f), left, right, back, front))
						continue;

					GenerateFaceVertices({ x, y, z}, block, static_cast<BlockFace>(f));
				}
			}
		}
	}

	for (uint32_t i = 0; i < m_IndexCount; i += 6)
	{
		GenerateFaceIndices();
	}

	m_Pushed = false;
}

void Chunk::PushData()
{
	if (!m_Pushed)
	{
		m_VertexArray = Quark::VertexArray::Create();

		auto vbo = Quark::VertexBuffer::Create((float*)m_Vertices.data(), m_Vertices.size() * sizeof(BlockVertex));
		vbo->SetLayout(s_BufferLayout);
		m_VertexArray->AddVertexBuffer(vbo);

		auto ibo = Quark::IndexBuffer::Create(m_Indices.data(), m_Indices.size());
		m_VertexArray->SetIndexBuffer(ibo);
	}
	
	m_Pushed = true;
}

Block Chunk::GenerateBlock(const glm::ivec3& position)
{
	static constexpr uint32_t stoneHeight = 58;
	static constexpr uint32_t dirtHeight = 63;
	static constexpr uint32_t grassHeight = 64;

	float noise = rand() / static_cast<float>(RAND_MAX);

	int32_t heightSample = m_HeightMap[position.z * ChunkSpecification::Depth + position.x];

	uint32_t genBedrock = static_cast<uint32_t>(1.0f + noise * 4.0f);
	uint32_t genStone = stoneHeight + heightSample;
	uint32_t genDirt = dirtHeight + heightSample;
	uint32_t genGrass = grassHeight + heightSample;

	Block type;
	if (position.y < genBedrock)
		type = Block::Bedrock;
	else if (position.y >= genBedrock && position.y < genStone)
		type = Block::Stone;
	else if (position.y >= genStone && position.y < genDirt)
		type = Block::Dirt;
	else if (position.y >= genDirt && position.y < genGrass)
		type = Block::GrassBlock;
	else
		type = Block::Air;

	return type;
}

void Chunk::GenerateFaceVertices(const glm::ivec3& position, Block type, BlockFace face)
{
	auto& blockProperties = Resources::GetBlockProperties().at(type);

	for (uint8_t i = 0; i < 4; i++)
	{
		BlockVertex v;
		v.Position = s_VertexPositions[i + static_cast<uint8_t>(face) * 4]
			+ glm::vec3(position.x + m_Coord.x * (float)ChunkSpecification::Width, position.y, position.z + m_Coord.y * (float)ChunkSpecification::Depth);
		v.TexCoord = blockProperties.Faces[static_cast<uint8_t>(face)].GetCoords()[i];
		v.Intensity = (static_cast<uint8_t>(face) + 1) / 6.0f;

		m_Vertices.push_back(v);
	}

	m_IndexCount += 6;
}

void Chunk::GenerateFaceIndices()
{
	m_Indices.push_back(0 + m_VertexCount);
	m_Indices.push_back(1 + m_VertexCount);
	m_Indices.push_back(3 + m_VertexCount);
	m_Indices.push_back(1 + m_VertexCount);
	m_Indices.push_back(2 + m_VertexCount);
	m_Indices.push_back(3 + m_VertexCount);

	m_VertexCount += 4;
}

Block Chunk::GetBlock(const glm::ivec3& position) const
{
	if (m_Blocks)
	{
		int32_t index = IndexAtPosition(position);
		if (index != -1)
		{
			return m_Blocks[index];
		}
	}
	return Block::None;
}

void Chunk::ReplaceBlock(const glm::ivec3& position, Block type)
{
	if (m_Blocks)
	{
		int32_t index = IndexAtPosition(position);
		if (index != -1)
		{
			m_Edited = true;

			m_Blocks[index] = type;

			m_World->OnChunkModified(m_Id);

			if (position.x == 0)
			{
				auto coord = m_Coord + glm::ivec2(-1, 0);
				m_World->OnChunkModified(CHUNK_UUID(coord));
			}
			else if (position.x == ChunkSpecification::Width - 1)
			{
				auto coord = m_Coord + glm::ivec2(1, 0);
				m_World->OnChunkModified(CHUNK_UUID(coord));
			}

			if (position.z == 0)
			{
				auto coord = m_Coord + glm::ivec2(0, -1);
				m_World->OnChunkModified(CHUNK_UUID(coord));
			}
			else if (position.z == ChunkSpecification::Depth - 1)
			{
				auto coord = m_Coord + glm::ivec2(0, 1);
				m_World->OnChunkModified(CHUNK_UUID(coord));
			}
		}
	}
}

bool Chunk::IsBlockFaceVisible(const glm::ivec3& position, BlockFace face, Chunk* left, Chunk* right, Chunk* back, Chunk* front) const
{
	glm::ivec3 neighborPosition = position + GetFaceNormal(face);

	bool visible = IsBlockTransparent(neighborPosition);
	
	// No further checks, face is culled
	if (!visible)
		return false;

	if (neighborPosition.x < 0)
	{
		auto& blockProperties = Resources::GetBlockProperties();

		Block block = left->GetBlock(glm::ivec3(ChunkSpecification::Width - 1, neighborPosition.y, neighborPosition.z));;
		if (!blockProperties.at(block).Transparent)
			visible = false;
	}
	else if (neighborPosition.x > ChunkSpecification::Width - 1)
	{
		auto& blockProperties = Resources::GetBlockProperties();

		Block block = right->GetBlock(glm::ivec3(0, neighborPosition.y, neighborPosition.z));
		if (!blockProperties.at(block).Transparent)
			visible = false;
	}

	if (neighborPosition.z < 0)
	{
		auto& blockProperties = Resources::GetBlockProperties();

		Block block = back->GetBlock(glm::ivec3(neighborPosition.x, neighborPosition.y, ChunkSpecification::Depth - 1));
		if (!blockProperties.at(block).Transparent)
			visible = false;
	}
	else if (neighborPosition.z > ChunkSpecification::Depth - 1)
	{
		auto& blockProperties = Resources::GetBlockProperties();

		Block block = front->GetBlock(glm::ivec3(neighborPosition.x, neighborPosition.y, 0));
		if (!blockProperties.at(block).Transparent)
			visible = false;
	}

	return visible;
}

bool Chunk::IsBlockTransparent(const glm::ivec3& position) const
{
	int32_t index = IndexAtPosition(position);
	if (index != -1)
	{
		auto& blockProperties = Resources::GetBlockProperties();
		return blockProperties.at(m_Blocks[index]).Transparent;
	}
	return true;
}
