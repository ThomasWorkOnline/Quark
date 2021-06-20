#include "WorldMap.h"

WorldMap::WorldMap(World* world)
	: m_World(world)
{

}

WorldMap::~WorldMap()
{
	std::lock_guard<std::recursive_mutex> lock(m_ChunksLocationsMutex);
	for (auto& chunk : m_ChunksLocations)
		delete chunk.second;
}

void WorldMap::Foreach(const std::function<void(Chunk* data)>& func) const
{
	std::lock_guard<std::recursive_mutex> lock(m_ChunksLocationsMutex);
	for (auto& e : m_ChunksLocations)
	{
		func(e.second);
	}
}

Chunk* WorldMap::Select(size_t id) const
{
	std::lock_guard<std::recursive_mutex> lock(m_ChunksLocationsMutex);
	auto it = m_ChunksLocations.find(id);

	if (it != m_ChunksLocations.end())
	{
		return it->second;
	}
	return nullptr;
}

Chunk* WorldMap::Load(size_t id)
{
	if (!Contains(id))
	{
		auto data = new Chunk(CHUNK_COORD(id), m_World);

		std::lock_guard<std::recursive_mutex> lock(m_ChunksLocationsMutex);
		m_ChunksLocations.insert({ id, data });
	}
	return Select(id);
}

void WorldMap::Unload(size_t id)
{
	auto data = Select(id);
	data->Save();

	Erase(id);
}

bool WorldMap::Contains(size_t id) const
{
	return Select(id);
}

void WorldMap::Erase(size_t id)
{
	auto data = Select(id);
	{
		std::lock_guard<std::recursive_mutex> lock(m_ChunksLocationsMutex);
		m_ChunksLocations.erase(id);
	}
	delete data;
}
