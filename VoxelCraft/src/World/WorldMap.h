#pragma once

#include "Chunk.h"

#include <mutex>
#include <unordered_map>

namespace VoxelCraft {

	class World;

	class WorldMap
	{
	public:
		WorldMap(World& world);

		void OnUpdate(float elapsedTime);

		void Foreach(const std::function<void(ChunkIdentifier id)>& func) const;
		void Foreach(const std::function<void(const Quark::Ref<Chunk>& data)>& func) const;

		uint32_t Count() const;

		Quark::Ref<Chunk> Select(ChunkIdentifier id) const;
		Quark::Ref<Chunk> Load(ChunkIdentifier id);
		void Unload(ChunkIdentifier id);
		bool Contains(ChunkIdentifier id) const;

	private:
		void Erase(const Quark::Ref<Chunk>& data);

	private:
		mutable std::recursive_mutex m_ChunksLocationsMutex;
		std::unordered_map<ChunkID, Quark::Ref<Chunk>> m_ChunksLocations;

		/// <summary>
		/// The deletion must be done on the main thread.
		/// Different Graphics Drivers might crash or ignore the call to glDelete<...>(),
		/// resulting in undefined behaviour. In most cases, a memory leak would occur.
		/// </summary>
		std::mutex m_ChunksToDeleteMutex;
		std::list<Quark::Ref<Chunk>> m_ChunksToDelete;

		World& m_World;
	};
}
