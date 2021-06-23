#pragma once

#include <Quark.h>

#include "Chunk.h"

#include <mutex>
#include <unordered_map>

class WorldMap
{
public:
	WorldMap();
	~WorldMap();

	void OnUpdate(float elapsedTime);

	void Foreach(const std::function<void(size_t id)>& func) const;
	void Foreach(const std::function<void(Chunk* data)>& func) const;

	Chunk* Select(size_t id) const;
	Chunk* Load(size_t id);
	void Unload(size_t id);
	bool Contains(size_t id) const;

private:
	void Erase(size_t id);

private:
	mutable std::recursive_mutex m_ChunksLocationsMutex;
	std::unordered_map<size_t, Chunk*> m_ChunksLocations;

	/// <summary>
	/// The deletion must be done on the main thread.
	/// Different Graphics Drivers might crash or ignore the call to glDelete<...>(),
	/// resulting in undefined behaviour. In most cases, a memory leak would occur.
	/// </summary>
	mutable std::mutex m_ChunksToDeleteMutex;
	std::list<Chunk*> m_ChunksToDelete;
};
