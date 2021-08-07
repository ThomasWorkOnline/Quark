#pragma once

#include "Chunk.h"
#include "WorldArea.h"

#include <mutex>
#include <thread>
#include <tuple>
#include <set>

namespace VoxelCraft {

	struct ChunkLoaderStats
	{
		uint32_t ChunksWorldGen = 0;
		uint32_t ChunksMeshGen = 0;
	};

	class World;

	class ChunkLoader
	{
	public:
		ChunkCoord Coord;
		uint32_t RenderDistance;
		ChunkLoaderStats Stats;

		ChunkLoader(World& world, ChunkCoord coord, uint32_t renderDistance);
		~ChunkLoader();

		void Start();
		void Stop();

		void OnUpdate(float elapsedTime);

		void Load(ChunkIdentifier id);
		void Unload(ChunkIdentifier id);

		bool Idling() const;

		static Quark::Scope<ChunkLoader> Create(World& world, ChunkCoord, uint32_t renderDistance = 8);

	private:
		void StartWork();
		void ProcessLoading();
		void ProcessUnloading();

		void LoadChunk(ChunkIdentifier id);
		void UnloadChunk(ChunkIdentifier id);

		void OnChunkBorderCrossed();

		void UniqueChunkDataGenerator(const Quark::Ref<Chunk>& chunk);
		void UniqueChunkMeshGenerator(const Quark::Ref<Chunk>& chunk, const ChunkNeighbors& neighbors);

	private:
		World& m_World;
		WorldArea m_LoadingArea;

		std::set<ChunkID> m_LoadingQueue;
		std::set<ChunkID> m_UnloadingQueue;
	};
}
