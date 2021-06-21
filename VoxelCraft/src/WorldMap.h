#pragma once

#include <Quark.h>

#include "Chunk.h"

#include <mutex>
#include <unordered_map>

class WorldMap
{
public:
	WorldMap(World* world);
	~WorldMap();

	void OnUpdate(float elapsedTime);

	void Foreach(const std::function<void(Chunk* data)>& func) const;

	Chunk* Select(size_t id) const;
	Chunk* Select(glm::ivec2 coord) const { return Select(CHUNK_UUID(coord)); }
	Chunk* Load(size_t id);
	Chunk* Load(glm::ivec2 coord) { return Load(CHUNK_UUID(coord)); }
	void Unload(size_t id);

	bool Contains(size_t id) const;

private:
	void Erase(size_t id);

private:
	mutable std::recursive_mutex m_ChunksLocationsMutex;
	std::unordered_map<size_t, Chunk*> m_ChunksLocations;

	mutable std::mutex m_ChunksToDeleteMutex;
	std::list<Chunk*> m_ChunksToDelete;

	World* m_World;
};
