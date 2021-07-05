#pragma once

#include <Quark.h>

#include "Chunk.h"
#include "WorldMap.h"

struct WorldAreaBounds
{
	glm::ivec2 First;
	glm::ivec2 Second;
};

class WorldArea
{
public:
	WorldArea(WorldMap& map, const glm::ivec2& size, const glm::ivec2& anchor);

	bool InBounds(size_t id) const;

	void Foreach(const std::function<void(size_t id)>& func) const;

	// TODO: improve, on fast movements, some chunks may stay loaded
	// if the player skips more than 1 chunk border in less than OnUpdate() timeframe
	void Invalidate(const glm::ivec2& size, const glm::ivec2& anchor);

private:
	void LoadArea();
	void UpdateOutOfBounds(const WorldAreaBounds& bounds, const WorldAreaBounds& lastBounds);

	void ComputeBounds(const glm::ivec2& size, const glm::ivec2& anchor);

	WorldAreaBounds m_AccessibleBounds; // Size in chunks. Only accessible chunks
	WorldAreaBounds m_InternalBounds; // +1 chunks on each side will be allocated

	WorldMap& m_WorldPartition;
};
