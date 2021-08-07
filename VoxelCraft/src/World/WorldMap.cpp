#include "WorldMap.h"

#include "World.h"

namespace VoxelCraft {

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

	uint32_t WorldMap::Count() const
	{
		std::lock_guard<std::recursive_mutex> lock(m_ChunksLocationsMutex);
		return m_ChunksLocations.size();
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
		if (!Contains(id))
		{
			auto data = Quark::CreateRef<Chunk>(m_World, id);
			m_ChunksLocations.insert({ id.ID, data });

			return data;
		}
		return Select(id);
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
