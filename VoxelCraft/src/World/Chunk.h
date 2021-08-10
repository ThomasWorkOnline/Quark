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
		ChunkNeighbors(const Quark::Ref<Chunk>& north, const Quark::Ref<Chunk>& south, const Quark::Ref<Chunk>& west, const Quark::Ref<Chunk>& east)
			: North(north), South(south), West(west), East(east) {}

		Quark::Ref<Chunk> North;
		Quark::Ref<Chunk> South;
		Quark::Ref<Chunk> West;
		Quark::Ref<Chunk> East;
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

		std::vector<SubChunk>::const_iterator begin() const { return SubChunks.begin(); }
		std::vector<SubChunk>::const_iterator end() const { return SubChunks.end(); }

		std::vector<SubChunk>::const_reverse_iterator rbegin() const { return SubChunks.rbegin(); }
		std::vector<SubChunk>::const_reverse_iterator rend() const { return SubChunks.rend(); }

		std::atomic<LoadStatus> LoadStatus = LoadStatus::Allocated;

		Chunk(World* world, ChunkIdentifier id);
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

		ChunkNeighbors QueryNeighbors() const;

		bool IsBlockTransparent(const IntPosition3D& position) const;
		bool IsBlockFaceVisible(const IntPosition3D& position, BlockFace face, const ChunkNeighbors& neighbors) const;

	private:
		Block GenerateBlock(const IntPosition3D& position);
		void RebuildSubMeshes(const IntPosition3D& position);

	private:
		Block* m_Blocks = nullptr;
		int32_t* m_HeightMap = nullptr;

		World* m_World;

		friend class SubChunk;
	};

	uint32_t IndexAtPosition(const IntPosition3D& position);
}
