#include "WorldMap.h"

#include "World.h"

namespace VoxelCraft {

	static size_t s_MaxBucketSize = 0;

	WorldMap::WorldMap(World& world)
		: m_World(world) {}

	void WorldMap::OnUpdate(float elapsedTime)
	{
		// TODO: Future me, is it possible that the ref count here
		// only gets decremented to 1? If so, this is undefined behaviour,
		// since the destructor might be called from a different thread.
		std::lock_guard<std::mutex> lock(m_ChunksToDeleteMutex);
		for (auto& chunk : m_ChunksToDelete)
		{
			chunk.reset();
		}
	}

	void WorldMap::Foreach(const std::function<void(ChunkIdentifier id)>& func) const
	{
		std::lock_guard<std::recursive_mutex> lock(m_ChunksLocationsMutex);
		for (auto& e : m_ChunksLocations)
		{
			func(e.first);
		}
	}

	void WorldMap::Foreach(const std::function<void(const Quark::Ref<Chunk>& data)>& func) const
	{
		std::lock_guard<std::recursive_mutex> lock(m_ChunksLocationsMutex);
		for (auto& e : m_ChunksLocations)
		{
			func(e.second);
		}
	}

	size_t WorldMap::Count() const
	{
		std::lock_guard<std::recursive_mutex> lock(m_ChunksLocationsMutex);
		return m_ChunksLocations.size();
	}

	size_t WorldMap::MaxBucketSize() const
	{
		return s_MaxBucketSize;
	}

	Quark::Ref<Chunk> WorldMap::Select(ChunkIdentifier id) const
	{
		std::lock_guard<std::recursive_mutex> lock(m_ChunksLocationsMutex);
		auto it = m_ChunksLocations.find(id.ID);

		if (it != m_ChunksLocations.end())
		{
			return it->second;
		}
		return nullptr;
	}

	Quark::Ref<Chunk> WorldMap::Load(ChunkIdentifier id)
	{
		// TODO: Check if the chunk is outside the writable areas ( -2 147 483 648 to +2 147 483 647 )

		std::lock_guard<std::recursive_mutex> lock(m_ChunksLocationsMutex);
		auto data = Select(id);
		if (!data)
		{
			data = Quark::CreateRef<Chunk>(m_World, id);
			m_ChunksLocations.emplace(id.ID, data);

			size_t bucket = m_ChunksLocations.bucket(id.ID);
			size_t size = m_ChunksLocations.bucket_size(bucket);
			s_MaxBucketSize = std::max(s_MaxBucketSize, size);
		}
		return data;
	}

	void WorldMap::Unload(ChunkIdentifier id)
	{
		auto data = Select(id);

		data->Save();

		// TODO: investigate the reason why a chunk can be null
		if (data)
			Erase(data);
	}

	bool WorldMap::Contains(ChunkIdentifier id) const
	{
		return Select(id) != nullptr;
	}

	void WorldMap::Erase(const Quark::Ref<Chunk>& data)
	{
		{
			std::lock_guard<std::recursive_mutex> lock(m_ChunksLocationsMutex);
			QK_ASSERT(data, "Hmmm");

			m_ChunksLocations.erase(data->ID.ID);
		}

		std::lock_guard<std::mutex> lock(m_ChunksToDeleteMutex);
		m_ChunksToDelete.push_back(data);
	}
}
