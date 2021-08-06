#pragma once

#include "ChunkIdentifier.h"
#include "SubChunk.h"

#include "../Blocks/Block.h"
#include "../Utils/Position.h"

#include <vector>

namespace VoxelCraft {

	struct ChunkSpecification
	{
		static constexpr int32_t SubChunksStackSize = 16;

		static constexpr int32_t Width  = SubChunkSpecification::Width; // x
		static constexpr int32_t Height = SubChunkSpecification::Height * SubChunksStackSize; // y
		static constexpr int32_t Depth  = SubChunkSpecification::Depth; // z

		static constexpr uint32_t BlockCount = Width * Height * Depth;
	};

	class World;
	class Chunk;

	struct ChunkNeighbors
	{
		// Order is important
		ChunkNeighbors(Chunk* north, Chunk* south, Chunk* west, Chunk* east)
			: North(north), South(south), West(west), East(east) {}

		Chunk* North;
		Chunk* South;
		Chunk* West;
		Chunk* East;
	};

	class Chunk
	{
	public:
		enum class LoadStatus : int8_t
		{
			Allocated = 0,
			WorldGenerating,
			WorldGenerated,
			Loading,
			Loaded,

			Occupied = WorldGenerating | Loading
		};

		ChunkIdentifier ID;
		std::vector<SubChunk> SubChunks;
		std::atomic<LoadStatus> LoadStatus = LoadStatus::Allocated;

		Chunk(World& world, ChunkIdentifier id);
		~Chunk();

		void Save() const;

		Block GetBlock(const IntPosition3D& position) const;
		bool ReplaceBlock(const IntPosition3D& position, Block type);

		void BuildTerrain();
		void BuildMesh(const ChunkNeighbors& neighbors);

		/// <summary>
		/// This function must be invoked from the main thread.
		/// </summary>
		void UploadMesh();

	private:
		Block GenerateBlock(const IntPosition3D& position);
		void RebuildSubMeshes(const IntPosition3D& position);

		bool IsBlockTransparent(const IntPosition3D& position) const;
		bool IsBlockFaceVisible(const IntPosition3D& position, BlockFace face, const ChunkNeighbors& neighbors) const;

		ChunkNeighbors QueryNeighbors() const;

	private:
		Block* m_Blocks = nullptr;
		int32_t* m_HeightMap = nullptr;

		World& m_World;

		friend class SubChunk;
		friend class ChunkMesh;
	};

	uint32_t IndexAtPosition(const IntPosition3D& position);
}
