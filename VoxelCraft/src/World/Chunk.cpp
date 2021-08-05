#include "Chunk.h"

#include <fstream>

#include "World.h"
#include "../Game/Resources.h"
#include "../Rendering/ChunkMesh.h"

namespace VoxelCraft {

	static uint32_t IndexAtPosition(const Position3D& position)
	{
		return position.x + ChunkSpecification::Width * position.z + ChunkSpecification::Width * ChunkSpecification::Depth * position.y;
	}

	static bool BoundsCheck(const Position3D& position)
	{
		return position.x >= 0 && position.y >= 0 && position.z >= 0 &&
			position.x < ChunkSpecification::Width && position.y < ChunkSpecification::Height && position.z < ChunkSpecification::Depth;
	}

	Chunk::Chunk(World& world, ChunkIdentifier id)
		: m_World(world), m_Id(id) {}

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

		Position2D heightIndex;
		for (heightIndex.y = 0; heightIndex.y < ChunkSpecification::Depth; heightIndex.y++)
		{
			for (heightIndex.x = 0; heightIndex.x < ChunkSpecification::Width; heightIndex.x++)
			{
				auto position = heightIndex.ToWorldSpace(m_Id.Coord);

				// Use deterministic algorithms for anything related to world gen
				// We need to assure the constancy for terrain generation and world resources
				float fValue = sinf(position.x * 0.144f + position.y * 0.021f) * 6.f;
				int32_t value = static_cast<int32_t>(round(fValue));
				uint32_t index = heightIndex.y * ChunkSpecification::Depth + heightIndex.x;
				m_HeightMap[index] = value;
			}
		}

		// Generate blocks
		m_Blocks = new Block[ChunkSpecification::BlockCount];

		Position3D position;
		for (position.y = 0; position.y < ChunkSpecification::Height; position.y++)
		{
			for (position.z = 0; position.z < ChunkSpecification::Depth; position.z++)
			{
				for (position.x = 0; position.x < ChunkSpecification::Width; position.x++)
				{
					m_Blocks[IndexAtPosition(position)] = GenerateBlock(position);
				}
			}
		}
	}

	void Chunk::BuildMesh(const ChunkNeighbors& neighbors)
	{
		m_Mesh.Create(this, neighbors);
		m_Pushed = false;
	}

	void Chunk::UploadMesh()
	{
		if (!m_Pushed)
		{
			m_Mesh.Upload();
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

		uint32_t index = position.z * ChunkSpecification::Depth + position.x;
		int32_t heightSample = m_HeightMap[index];

		uint32_t genBedrock = static_cast<uint32_t>(1.0f + noise * 4.0f);
		uint32_t genStone = stoneHeight + heightSample;
		uint32_t genDirt = dirtHeight + heightSample;
		uint32_t genGrassBlock = grassBlockHeight + heightSample;
		uint32_t genGrass = grassHeight + heightSample;

		Block type;
		if (position.y < genBedrock)
			type = Block::ID::Bedrock;
		else if (position.y >= genBedrock && position.y < genStone)
			type = Block::ID::Stone;
		else if (position.y >= genStone && position.y < genDirt)
			type = Block::ID::Dirt;
		else if (position.y >= genDirt && position.y < genGrassBlock)
			type = Block::ID::GrassBlock;
		else if (position.y >= genGrassBlock && position.y < genGrass)
			type = noise < 0.01f ? Block::ID::Poppy : noise > 0.9f ? Block::ID::Grass : Block::ID::Air;
		else
			type = Block::ID::Air;

		return type;
	}

	Block Chunk::GetBlock(const Position3D& position) const
	{
		if (BoundsCheck(position))
		{
			uint32_t index = IndexAtPosition(position);
			return m_Blocks[index];
		}
		return Block::ID::Air;
	}

	void Chunk::ReplaceBlock(const Position3D& position, Block type)
	{
		if (BoundsCheck(position))
		{
			uint32_t index = IndexAtPosition(position);

			m_Edited = true;

			m_Blocks[index] = type;

			m_World.OnChunkModified(m_Id);

			if (position.x == 0)
			{
				auto west = m_Id.Coord.West();
				m_World.OnChunkModified(west);
			}
			else if (position.x == ChunkSpecification::Width - 1)
			{
				auto east = m_Id.Coord.East();
				m_World.OnChunkModified(east);
			}

			if (position.z == 0)
			{
				auto south = m_Id.Coord.South();
				m_World.OnChunkModified(south);
			}
			else if (position.z == ChunkSpecification::Depth - 1)
			{
				auto north = m_Id.Coord.North();
				m_World.OnChunkModified(north);
			}
		}
	}

	bool Chunk::IsBlockFaceVisible(const Position3D& position, BlockFace face, const ChunkNeighbors& neighbors) const
	{
		Position3D neighborPosition = position + GetFaceNormal(face); // In chunk space

		if (BoundsCheck(neighborPosition))
		{
			return IsBlockTransparent(neighborPosition);
		}

		if (neighborPosition.x < 0)
		{
			Block block = neighbors.West->GetBlock(Position3D((float)ChunkSpecification::Width - 1, neighborPosition.y, neighborPosition.z));
			if (!block.GetProperties().Transparent)
				return false;
		}
		else if (neighborPosition.x > ChunkSpecification::Width - 1)
		{
			Block block = neighbors.East->GetBlock(Position3D(0.f, neighborPosition.y, neighborPosition.z));
			if (!block.GetProperties().Transparent)
				return false;
		}

		if (neighborPosition.z < 0)
		{
			Block block = neighbors.South->GetBlock(Position3D(neighborPosition.x, neighborPosition.y, (float)ChunkSpecification::Depth - 1));
			if (!block.GetProperties().Transparent)
				return false;
		}
		else if (neighborPosition.z > ChunkSpecification::Depth - 1)
		{
			Block block = neighbors.North->GetBlock(Position3D(neighborPosition.x, neighborPosition.y, 0.f));
			if (!block.GetProperties().Transparent)
				return false;
		}

		return true;
	}

	bool Chunk::IsBlockTransparent(const Position3D& position) const
	{
		uint32_t index = IndexAtPosition(position);
		return m_Blocks[index].GetProperties().Transparent;
	}
}
