#include "WorldMap.h"

#include "World.h"

namespace VoxelCraft {

	WorldMap::WorldMap(World& world)
		: m_World(world) {}

	WorldMap::~WorldMap()
	{
		std::lock_guard<std::recursive_mutex> lock(m_ChunksLocationsMutex);
		for (auto& chunk : m_ChunksLocations)
			delete chunk.second;
	}

	void WorldMap::OnUpdate(float elapsedTime)
	{
		{
			std::lock_guard<std::mutex> lock(m_ChunksToDeleteMutex);
			for (auto data : m_ChunksToDelete)
			{
				delete data;
			}
			m_ChunksToDelete.clear();
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

	void WorldMap::Foreach(const std::function<void(Chunk* data)>& func) const
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

	Chunk* WorldMap::Select(ChunkIdentifier id) const
	{
		std::lock_guard<std::recursive_mutex> lock(m_ChunksLocationsMutex);
		auto it = m_ChunksLocations.find(id.ID);

		if (it != m_ChunksLocations.end())
		{
			return it->second;
		}
		return nullptr;
	}

	Chunk* WorldMap::Load(ChunkIdentifier id)
	{
		// TODO: Check if the chunk is outside the writable areas ( -2 147 483 648 to +2 147 483 647 )

		std::lock_guard<std::recursive_mutex> lock(m_ChunksLocationsMutex);
		if (!Contains(id))
		{
			auto data = new Chunk(m_World, id);

			m_ChunksLocations.insert({ id.ID, data });

			return data;
		}
		return Select(id);
	}

	void WorldMap::Unload(ChunkIdentifier id)
	{
		auto data = Select(id);
		data->Save();

		Erase(id);
	}

	bool WorldMap::Contains(ChunkIdentifier id) const
	{
		return Select(id);
	}

	void WorldMap::Erase(ChunkIdentifier id)
	{
		auto data = Select(id);
		{
			std::lock_guard<std::recursive_mutex> lock(m_ChunksLocationsMutex);
			m_ChunksLocations.erase(id.ID);
		}

		std::lock_guard<std::mutex> lock(m_ChunksToDeleteMutex);
		m_ChunksToDelete.push_back(data);
	}
}
