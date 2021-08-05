#include "Chunk.h"

#include <fstream>

#include "World.h"
#include "../Game/Resources.h"
#include "../Rendering/ChunkMesh.h"

namespace VoxelCraft {

	static uint32_t IndexAtPosition(const IntPosition3D& position)
	{
		return position.x + ChunkSpecification::Width * position.z + ChunkSpecification::Width * ChunkSpecification::Depth * position.y;
	}

	static bool BoundsCheck(const IntPosition3D& position)
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
				double dValue = sin(position.x * 0.144 + position.y * 0.021) * 6.0;
				int32_t value = static_cast<int32_t>(round(dValue));
				uint32_t index = heightIndex.y * ChunkSpecification::Depth + heightIndex.x;
				m_HeightMap[index] = value;
			}
		}

		// Generate blocks
		m_Blocks = new Block[ChunkSpecification::BlockCount];

		IntPosition3D position;
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

	Block Chunk::GenerateBlock(const IntPosition3D& position)
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

	Block Chunk::GetBlock(const IntPosition3D& position) const
	{
		if (BoundsCheck(position))
		{
			uint32_t index = IndexAtPosition(position);
			return m_Blocks[index];
		}
		return Block::ID::Air;
	}

	void Chunk::ReplaceBlock(const IntPosition3D& position, Block type)
	{
		if (BoundsCheck(position))
		{
			uint32_t index = IndexAtPosition(position);

			m_Edited = true;

			m_Blocks[index] = type;

			m_World.OnChunkModified(m_Id);

			if (position.x == 0)
			{
				m_World.OnChunkModified(m_Id.West());
			}
			else if (position.x == ChunkSpecification::Width - 1)
			{
				m_World.OnChunkModified(m_Id.East());
			}

			if (position.z == 0)
			{
				m_World.OnChunkModified(m_Id.South());
			}
			else if (position.z == ChunkSpecification::Depth - 1)
			{
				m_World.OnChunkModified(m_Id.North());
			}
		}
	}

	bool Chunk::IsBlockFaceVisible(const IntPosition3D& position, BlockFace face, const ChunkNeighbors& neighbors) const
	{
		IntPosition3D neighborPosition = position + GetFaceNormal(face); // In chunk space

		if (BoundsCheck(neighborPosition))
		{
			return IsBlockTransparent(neighborPosition);
		}

		if (neighborPosition.x < 0)
		{
			Block block = neighbors.West->GetBlock({ ChunkSpecification::Width - 1, neighborPosition.y, neighborPosition.z });
			if (!block.GetProperties().Transparent)
				return false;
		}
		else if (neighborPosition.x > ChunkSpecification::Width - 1)
		{
			Block block = neighbors.East->GetBlock({ 0, neighborPosition.y, neighborPosition.z });
			if (!block.GetProperties().Transparent)
				return false;
		}

		if (neighborPosition.z < 0)
		{
			Block block = neighbors.South->GetBlock({ neighborPosition.x, neighborPosition.y, ChunkSpecification::Depth - 1 });
			if (!block.GetProperties().Transparent)
				return false;
		}
		else if (neighborPosition.z > ChunkSpecification::Depth - 1)
		{
			Block block = neighbors.North->GetBlock({ neighborPosition.x, neighborPosition.y, 0 });
			if (!block.GetProperties().Transparent)
				return false;
		}

		return true;
	}

	bool Chunk::IsBlockTransparent(const IntPosition3D& position) const
	{
		uint32_t index = IndexAtPosition(position);
		return m_Blocks[index].GetProperties().Transparent;
	}
}
