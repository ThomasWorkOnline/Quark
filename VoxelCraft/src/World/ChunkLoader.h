#pragma once

#include "Chunk.h"
#include "WorldArea.h"

#include <list>
#include <mutex>
#include <thread>
#include <tuple>

namespace VoxelCraft {

	struct ChunkLoaderStats
	{
		uint32_t ChunksAllocated = 0;
		uint32_t ChunksFreed = 0;
		uint32_t ChunksWorldGen = 0;
		uint32_t ChunksMeshGen = 0;
	};

	class World;

	class ChunkLoader
	{
	public:
		ChunkLoader(World& world, ChunkCoord coord, uint32_t renderDistance);
		~ChunkLoader();

		void OnUpdate(float elapsedTime);

		void SetCoord(ChunkCoord coord) { m_Coord = coord; }
		ChunkCoord GetCoord() const { return m_Coord; }

		void Load(ChunkID id);
		void Rebuild(ChunkID id);

		bool Idling() const;

		const ChunkLoaderStats& GetStats() const { return m_Stats; }

		static Quark::Scope<ChunkLoader> Create(World& world, ChunkCoord, uint32_t renderDistance = 8);

	private:
		void StartWork();
		void ProcessLoading();

		void LoadChunk(ChunkID id);
		void UnloadChunk(ChunkID id);

		void OnChunkBorderCrossed();

		void UniqueChunkDataGenerator(Chunk* chunk);
		void UniqueChunkMeshGenerator(Chunk* chunk, Chunk* left, Chunk* right, Chunk* back, Chunk* front);

		World& m_World;
		WorldArea m_LoadingArea;

		std::list<ChunkID> m_LoadingQueue;

		ChunkLoaderStats m_Stats;

		ChunkCoord m_Coord;
		uint32_t m_RenderDistance;
	};
}
