#include "Chunk.h"

#include <fstream>

#include "World.h"
#include "Meshes.h"

static int32_t IndexAtPosition(const glm::ivec3& position)
{
	if (position.x < 0 || position.y < 0 || position.z < 0
		|| position.x >= ChunkSpecification::Width || position.y >= ChunkSpecification::Height || position.z >= ChunkSpecification::Depth)
		return -1;
	return position.x + ChunkSpecification::Width * position.z + ChunkSpecification::Width * ChunkSpecification::Depth * position.y;
}

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

				GenerateBlockMesh({ x, y, z }, block, { left, right, back, front });
			}
		}
	}

	m_Pushed = false;
}

void Chunk::PushData()
{
	if (!m_Pushed)
	{
		m_VertexArray = Quark::VertexArray::Create();

		auto vbo = Quark::VertexBuffer::Create((float*)m_Vertices.data(), m_Vertices.size() * sizeof(Vertex));
		vbo->SetLayout(Resources::GetBufferLayout());
		m_VertexArray->AddVertexBuffer(vbo);

		auto ibo = Quark::IndexBuffer::Create(m_Indices.data(), m_Indices.size());
		m_VertexArray->SetIndexBuffer(ibo);
	}
	
	m_Pushed = true;
}

Block Chunk::GenerateBlock(const Position3D& position)
{
	static constexpr uint32_t stoneHeight = 58;
	static constexpr uint32_t dirtHeight = 63;
	static constexpr uint32_t grassBlockHeight = 64;
	static constexpr uint32_t grassHeight = 65;

	float noise = rand() / static_cast<float>(RAND_MAX);

	int32_t heightSample = m_HeightMap[position.z * ChunkSpecification::Depth + position.x];

	uint32_t genBedrock = static_cast<uint32_t>(1.0f + noise * 4.0f);
	uint32_t genStone = stoneHeight + heightSample;
	uint32_t genDirt = dirtHeight + heightSample;
	uint32_t genGrassBlock = grassBlockHeight + heightSample;
	uint32_t genGrass = grassHeight + heightSample;

	Block type;
	if (position.y < genBedrock)
		type = Block::Bedrock;
	else if (position.y >= genBedrock && position.y < genStone)
		type = Block::Stone;
	else if (position.y >= genStone && position.y < genDirt)
		type = Block::Dirt;
	else if (position.y >= genDirt && position.y < genGrassBlock)
		type = Block::GrassBlock;
	else if (position.y >= genGrassBlock && position.y < genGrass)
		type = noise < 0.01f ? Block::Poppy : Block::Air;
	else
		type = Block::Air;

	return type;
}

void Chunk::GenerateBlockMesh(const Position3D& position, Block type, const Neighbors& neighbors)
{
	auto& blockProperties = Resources::GetBlockProperties(type);

	switch (blockProperties.Mesh)
	{
	case MeshModel::Block:
		// Create all the faces
		for (uint8_t f = 0; f < 6; f++)
		{
			if (!IsBlockFaceVisible(position, BlockFace(f), neighbors))
				continue;

			BlockMesh::CreateFace(m_Vertices, m_Indices, m_VertexCount, position.ToWorldSpace(m_Coord), blockProperties, BlockFace(f));
		}
		break;
	case MeshModel::CrossSprite:
		CrossSpriteMesh::Create(m_Vertices, m_Indices, m_VertexCount, position.ToWorldSpace(m_Coord), blockProperties);
		break;
	}
}

Block Chunk::GetBlock(const Position3D& position) const
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

void Chunk::ReplaceBlock(const Position3D& position, Block type)
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
				auto coord = m_Coord + Position2D(-1, 0);
				m_World->OnChunkModified(CHUNK_UUID(coord));
			}
			else if (position.x == ChunkSpecification::Width - 1)
			{
				auto coord = m_Coord + Position2D(1, 0);
				m_World->OnChunkModified(CHUNK_UUID(coord));
			}

			if (position.z == 0)
			{
				auto coord = m_Coord + Position2D(0, -1);
				m_World->OnChunkModified(CHUNK_UUID(coord));
			}
			else if (position.z == ChunkSpecification::Depth - 1)
			{
				auto coord = m_Coord + Position2D(0, 1);
				m_World->OnChunkModified(CHUNK_UUID(coord));
			}
		}
	}
}

bool Chunk::IsBlockFaceVisible(const Position3D& position, BlockFace face, const Neighbors& neighbors) const
{
	Position3D neighborPosition = position + GetFaceNormal(face);

	bool visible = IsBlockTransparent(neighborPosition);
	
	// No further checks, face is culled
	if (!visible)
		return false;

	if (neighborPosition.x < 0)
	{

		Block block = neighbors.Left->GetBlock(glm::ivec3(ChunkSpecification::Width - 1, neighborPosition.y, neighborPosition.z));;
		auto& blockProperties = Resources::GetBlockProperties(block);
		if (!blockProperties.Transparent)
			visible = false;
	}
	else if (neighborPosition.x > ChunkSpecification::Width - 1)
	{
		Block block = neighbors.Right->GetBlock(glm::ivec3(0, neighborPosition.y, neighborPosition.z));
		auto& blockProperties = Resources::GetBlockProperties(block);
		if (!blockProperties.Transparent)
			visible = false;
	}

	if (neighborPosition.z < 0)
	{
		Block block = neighbors.Back->GetBlock(glm::ivec3(neighborPosition.x, neighborPosition.y, ChunkSpecification::Depth - 1));
		auto& blockProperties = Resources::GetBlockProperties(block);
		if (!blockProperties.Transparent)
			visible = false;
	}
	else if (neighborPosition.z > ChunkSpecification::Depth - 1)
	{
		Block block = neighbors.Front->GetBlock(glm::ivec3(neighborPosition.x, neighborPosition.y, 0));
		auto& blockProperties = Resources::GetBlockProperties(block);
		if (!blockProperties.Transparent)
			visible = false;
	}

	return visible;
}

bool Chunk::IsBlockTransparent(const Position3D& position) const
{
	int32_t index = IndexAtPosition(position);
	if (index != -1)
	{
		return Resources::GetBlockProperties(m_Blocks[index]).Transparent;
	}
	return true;
}
