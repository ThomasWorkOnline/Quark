#include "Chunk.h"

#include <fstream>
#include <random>

#include "World.h"
#include "../Game/Resources.h"
#include "../Rendering/ChunkMesh.h"

namespace VoxelCraft {

	static std::mt19937 s_Random;

	static bool BoundsCheck(const IntPosition3D& position)
	{
		return position.x >= 0 && position.y >= 0 && position.z >= 0 &&
			position.x < ChunkSpecification::Width && position.y < ChunkSpecification::Height && position.z < ChunkSpecification::Depth;
	}

	static void DumpAsBinary(std::ofstream& out, const void* data, size_t size)
	{
		out.write(reinterpret_cast<const char*>(data), size);
	}

	Chunk::Chunk(World* world, ChunkIdentifier id)
		: ID(id), m_World(world)
	{
		SubChunks.reserve(ChunkSpecification::SubChunksStackSize);

		for (int32_t i = 0; i < ChunkSpecification::SubChunksStackSize; i++)
		{
			SubChunks.emplace_back(this, i);
		}
	}

	Chunk::~Chunk()
	{
		delete[] m_Blocks;
		delete[] m_HeightMap;

		QK_ASSERT(Quark::Application::Get().GetThreadID() == std::this_thread::get_id(), "Invalid delete");
	}

	void Chunk::Save() const
	{
		
	}

	Block Chunk::GetBlock(const IntPosition3D& position) const
	{
		if (m_Blocks && BoundsCheck(position))
		{
			uint32_t index = IndexAtPosition(position);
			return m_Blocks[index];
		}
		return BlockID::Air;
	}

	bool Chunk::ReplaceBlock(const IntPosition3D& position, Block type)
	{
		if (!(m_Blocks && BoundsCheck(position)))
			return false;

		uint32_t index = IndexAtPosition(position);
		m_Blocks[index] = type;

		RebuildSubMeshes(position);

		return true;
	}

	void Chunk::BuildTerrain()
	{
		QK_ASSERT(m_Blocks == nullptr, "Tried to allocate blocks more than once");

		s_Random.seed(ID.Coord.x | ID.Coord.y);

		// Generate height map
		m_HeightMap = new int32_t[ChunkSpecification::Width * ChunkSpecification::Depth];

		IntPosition2D heightIndex;
		for (heightIndex.y = 0; heightIndex.y < ChunkSpecification::Depth; heightIndex.y++)
		{
			for (heightIndex.x = 0; heightIndex.x < ChunkSpecification::Width; heightIndex.x++)
			{
				auto position = heightIndex.ToWorldSpace(ID.Coord);

				//double noise = s_Random() / static_cast<double>(std::numeric_limits<unsigned int>::max());

				// Use deterministic algorithms for anything related to world gen
				// We need to assure the constancy for terrain generation and world resources
				double dValue = sin(position.x * 0.144 + position.y * 0.021) * 6.0;
				int32_t value = static_cast<int32_t>(round(dValue));
				int32_t index = heightIndex.y * ChunkSpecification::Depth + heightIndex.x;
				m_HeightMap[index] = value;
			}
		}

		// Generate blocks
		m_Blocks = new Block[ChunkSpecification::BlockCount];

		for (auto& subChunk : SubChunks)
		{
			subChunk.BuildTerrain();
		}
	}

	void Chunk::BuildMesh(const ChunkNeighbors& neighbors)
	{
		for (auto& subChunk : SubChunks)
		{
			if (subChunk.HasData())
				subChunk.Mesh.Create(subChunk, neighbors);
		}
	}

	void Chunk::UploadMesh()
	{
		QK_ASSERT(Quark::Application::Get().GetThreadID() == std::this_thread::get_id(), "Invalid caller thread");

		for (auto& subChunk : SubChunks)
		{
			auto& mesh = subChunk.Mesh;

			if (mesh.RequiresUpload())
				mesh.Upload();
		}
	}

	ChunkNeighbors Chunk::QueryNeighbors() const
	{
		// Force load if it doesn't exist to prevent crashes
		return {
			m_World->Map.Create(ID.North()),
			m_World->Map.Create(ID.South()),
			m_World->Map.Create(ID.West()),
			m_World->Map.Create(ID.East())
		};
	}

	bool Chunk::IsBlockTransparent(const IntPosition3D& position) const
	{
		uint32_t index = IndexAtPosition(position);
		return m_Blocks[index].GetProperties().Transparent;
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

	Block Chunk::GenerateBlock(const IntPosition3D& position)
	{
		static constexpr uint32_t stoneHeight = 58;
		static constexpr uint32_t dirtHeight = 63;
		static constexpr uint32_t grassBlockHeight = 64;
		static constexpr uint32_t grassHeight = 65;

		float noise = s_Random() / static_cast<float>(std::numeric_limits<unsigned int>::max());

		int32_t index = position.z * ChunkSpecification::Depth + position.x;
		int32_t heightSample = m_HeightMap[index];

		int32_t genBedrock = static_cast<uint32_t>(1.0f + noise * 4.0f);
		int32_t genStone = stoneHeight + heightSample;
		int32_t genDirt = dirtHeight + heightSample;
		int32_t genGrassBlock = grassBlockHeight + heightSample;
		int32_t genGrass = grassHeight + heightSample;

		Block type;
		if (position.y < genBedrock)
			type = BlockID::Bedrock;
		else if (position.y >= genBedrock && position.y < genStone)
			type = BlockID::Stone;
		else if (position.y >= genStone && position.y < genDirt)
			type = BlockID::Dirt;
		else if (position.y >= genDirt && position.y < genGrassBlock)
			type = BlockID::GrassBlock;
		else if (position.y >= genGrassBlock && position.y < genGrass)
			type = noise < 0.01f ? BlockID::Poppy : noise > 0.9f ? BlockID::Grass : BlockID::Air;
		else
			type = BlockID::Air;

		return type;
	}

	void Chunk::RebuildSubMeshes(const IntPosition3D& position)
	{
		uint32_t subChunkStackIndex = position.y / SubChunkSpecification::Height;

		SubChunk& center = SubChunks[subChunkStackIndex];
		Position3D pos = center.GetPositionFromParentChunk(position);

		ChunkNeighbors neighbors = QueryNeighbors();
		center.RebuildMesh(neighbors);

		if (pos.x == 0)
		{
			auto west = neighbors.West;
			SubChunk& subEast = west->SubChunks[subChunkStackIndex];
			subEast.RebuildMesh(west->QueryNeighbors());
		}
		else if (pos.x == SubChunkSpecification::Width - 1)
		{
			const auto east = neighbors.East;
			SubChunk& subWest = east->SubChunks[subChunkStackIndex];
			subWest.RebuildMesh(east->QueryNeighbors());
		}

		if (pos.y == 0)
		{
			if (subChunkStackIndex > 0)
			{
				SubChunk& bottom = SubChunks[subChunkStackIndex - 1];
				bottom.RebuildMesh(neighbors);
			}
		}
		else if (pos.y == SubChunkSpecification::Height - 1)
		{
			if (subChunkStackIndex < ChunkSpecification::SubChunksStackSize - 1)
			{
				SubChunk& top = SubChunks[subChunkStackIndex + 1];
				top.RebuildMesh(neighbors);
			}
		}

		if (pos.z == 0)
		{
			const auto south = neighbors.South;
			SubChunk& subSouth = south->SubChunks[subChunkStackIndex];
			subSouth.RebuildMesh(south->QueryNeighbors());
		}
		else if (pos.z == SubChunkSpecification::Depth - 1)
		{
			const auto north = neighbors.North;
			SubChunk& subNorth = north->SubChunks[subChunkStackIndex];
			subNorth.RebuildMesh(north->QueryNeighbors());
		}
	}

	uint32_t IndexAtPosition(const IntPosition3D& position)
	{
		return position.x + ChunkSpecification::Width * position.z + ChunkSpecification::Width * ChunkSpecification::Depth * position.y;
	}
}
