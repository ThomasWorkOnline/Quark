#pragma once

#include "Chunk.h"
#include "WorldArea.h"

#include <mutex>
#include <thread>
#include <tuple>
#include <set>

namespace VoxelCraft {

	class World;

	class ChunkLoader
	{
	public:
		ChunkCoord Coord;
		uint32_t RenderDistance;

		ChunkLoader(World* world, ChunkCoord coord, uint32_t renderDistance);
		~ChunkLoader();

		void Start();
		void Stop();

		void Invalidate();

		void Load(ChunkIdentifier id);
		void Unload(ChunkIdentifier id);

		bool Idling() const;

	private:
		void Work();
		void ProcessLoading();
		void ProcessUnloading();

		void OnIdle();
		void OnResume();

	private:
		World* m_World;
		WorldArea m_LoadingArea;

		std::set<ChunkID> m_LoadingQueue;
		std::set<ChunkID> m_UnloadingQueue;
	};
}
