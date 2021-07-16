#pragma once

#include <Quark.h>

#include "Chunk.h"

#include <mutex>
#include <unordered_map>

namespace VoxelCraft {

	class World;

	class WorldMap
	{
	public:
		WorldMap(World& world);
		~WorldMap();

		void OnUpdate(float elapsedTime);

		void Foreach(const std::function<void(ChunkID id)>& func) const;
		void Foreach(const std::function<void(Chunk* data)>& func) const;

		Chunk* Select(ChunkID id) const;
		Chunk* Load(ChunkID id);
		void Unload(ChunkID id);
		bool Contains(ChunkID id) const;

	private:
		void Erase(ChunkID id);

	private:
		mutable std::recursive_mutex m_ChunksLocationsMutex;
		std::unordered_map<ChunkID, Chunk*> m_ChunksLocations;

		/// <summary>
		/// The deletion must be done on the main thread.
		/// Different Graphics Drivers might crash or ignore the call to glDelete<...>(),
		/// resulting in undefined behaviour. In most cases, a memory leak would occur.
		/// </summary>
		mutable std::mutex m_ChunksToDeleteMutex;
		std::list<Chunk*> m_ChunksToDelete;

		World& m_World;
	};
}