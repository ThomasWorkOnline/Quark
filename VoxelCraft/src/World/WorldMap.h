#pragma once

#include "Chunk.h"

#include <mutex>
#include <unordered_map>

namespace VoxelCraft {

	class World;

	class WorldMap
	{
	public:
		WorldMap(World* world);

		// TODO: change

		/// <summary>
		/// This function must be invoked from the main thread.
		/// </summary>
		/// <param name="elapsedTime"></param>
		void ProcessDeletion();

		void Foreach(const std::function<void(ChunkIdentifier id)>& func) const;
		void Foreach(const std::function<void(const Quark::Ref<Chunk>& data)>& func) const;

		size_t Count() const;
		size_t MaxBucketSize() const;

		/// <summary>
		/// Gets the chunk associated with the specified identifier.
		/// </summary>
		/// <param name="id"></param>
		/// <returns>nullptr if no chunk was found.</returns>
		Quark::Ref<Chunk> Get(ChunkIdentifier id) const;

		void Load(ChunkIdentifier id);
		void Load(const Quark::Ref<Chunk>& data);
		void Unload(ChunkIdentifier id);
		void Unload(const Quark::Ref<Chunk>& data);

		bool Contains(ChunkIdentifier id) const;

		/// <summary>
		/// The neighbors around the chunk specified by the identifier.
		/// If one or more neighbors are nullptr, they shall get created.
		/// </summary>
		/// <param name="id"></param>
		/// <returns></returns>
		ChunkNeighbors GetNonNullNeighbors(ChunkIdentifier id);

	private:
		Quark::Ref<Chunk> Create(ChunkIdentifier id);
		void Erase(const Quark::Ref<Chunk>& data);

	private:
		mutable std::mutex m_ChunksLocationsMutex;
		std::unordered_map<ChunkID, Quark::Ref<Chunk>> m_ChunksLocations;

		/// <summary>
		/// The deletion must be done on the main thread.
		/// Different Graphics Drivers might crash or ignore the call to glDelete<...>(),
		/// resulting in undefined behaviour. In most cases, a memory leak would occur.
		/// </summary>
		std::mutex m_ChunksToDeleteMutex;
		std::list<Quark::Ref<Chunk>> m_ChunksToDelete;

		World* m_World;
	};
}
