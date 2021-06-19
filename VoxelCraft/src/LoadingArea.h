#pragma once

#include <Quark.h>

#include "Chunk.h"
#include "WorldMap.h"

#include <mutex>
#include <unordered_set>

static struct LoadingAreaBounds
{
	glm::ivec2 First;
	glm::ivec2 Second;
};

class LoadingArea
{
public:
	LoadingArea(WorldMap& map, const glm::ivec2& size, const glm::ivec2& anchor);

	bool InBounds(glm::ivec2 coord) const;

	void Foreach(const std::function<void(glm::ivec2 coord)>& func) const;
	void Foreach(const std::function<void(Chunk* data)>& func) const;
	void OnUnload(const std::function<void(glm::ivec2 coord)>& func);

	void Invalidate(const glm::ivec2& size, const glm::ivec2& anchor);

private:
	void LoadArea();
	void UnloadOutOfBounds(const LoadingAreaBounds& bounds, const LoadingAreaBounds& lastBounds);

	void ComputeBounds(const glm::ivec2& size, const glm::ivec2& anchor);

	LoadingAreaBounds m_AccessibleBounds; // In chunks. Only accessible, +1 chunks on each side will be allocated
	LoadingAreaBounds m_InternalBounds;

	mutable std::mutex m_ChunksToFreeMutex;
	std::unordered_set<size_t> m_ChunksToUnload;

	WorldMap& m_WorldPartition;
};
