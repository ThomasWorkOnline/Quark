#include "WorldArea.h"

WorldArea::WorldArea(WorldMap& map, const glm::ivec2& size, const glm::ivec2& anchor)
	: m_WorldPartition(map)
{
	Invalidate(size, anchor);
}

bool WorldArea::InBounds(glm::ivec2 coord) const
{
	return (coord.x >= m_AccessibleBounds.First.x && coord.x <= m_AccessibleBounds.Second.x &&
		coord.y >= m_AccessibleBounds.First.y && coord.y <= m_AccessibleBounds.Second.y);
}

void WorldArea::Foreach(const std::function<void(glm::ivec2 coord)>& func) const
{
	for (int z = m_AccessibleBounds.First.y; z < m_AccessibleBounds.Second.y; z++)
	{
		for (int x = m_AccessibleBounds.First.x; x < m_AccessibleBounds.Second.x; x++)
		{
			func({ x, z });
		}
	}
}

void WorldArea::OnUnload(const std::function<void(glm::ivec2 coord)>& func)
{
	std::lock_guard<std::mutex> lock(m_ChunksToUnloadMutex);
	for (auto e : m_ChunksToUnload)
	{
		func(CHUNK_COORD(e));
	}

	m_ChunksToUnload.clear();
}

void WorldArea::Invalidate(const glm::ivec2& size, const glm::ivec2& anchor)
{
	static LoadingAreaBounds lastBounds = m_InternalBounds;

	ComputeBounds(size, anchor);
	LoadArea();
	UnloadOutOfBounds(m_InternalBounds, lastBounds);

	lastBounds = m_InternalBounds;
}

void WorldArea::LoadArea()
{
	for (int z = m_InternalBounds.First.y; z < m_InternalBounds.Second.y; z++)
	{
		for (int x = m_InternalBounds.First.x; x < m_InternalBounds.Second.x; x++)
		{
			glm::ivec2 coord(x, z);
			m_WorldPartition.Load(CHUNK_UUID(coord));
		}
	}
}

void WorldArea::UnloadOutOfBounds(const LoadingAreaBounds& bounds, const LoadingAreaBounds& lastBounds)
{
	std::unordered_set<size_t> coords;

	for (int z = bounds.First.y; z < bounds.Second.y; z++)
	{
		for (int x = bounds.First.x; x < bounds.Second.x; x++)
		{
			glm::ivec2 coord(x, z);
			coords.insert(CHUNK_UUID(coord));
		}
	}

	for (int z = lastBounds.First.y; z < lastBounds.Second.y; z++)
	{
		for (int x = lastBounds.First.x; x < lastBounds.Second.x; x++)
		{
			glm::ivec2 coord = { x, z };
			auto it = coords.find(CHUNK_UUID(coord));
			if (it == coords.end())
			{
				std::lock_guard<std::mutex> lock(m_ChunksToUnloadMutex);
				m_ChunksToUnload.insert((CHUNK_UUID(coord)));
			}
		}
	}
}

void WorldArea::ComputeBounds(const glm::ivec2& size, const glm::ivec2& anchor)
{
	m_AccessibleBounds.First = { anchor.x - size.x, anchor.y - size.y };
	m_AccessibleBounds.Second = { anchor.x + size.x, anchor.y + size.y };

	m_InternalBounds.First = { anchor.x - size.x - 1, anchor.y - size.y - 1 };
	m_InternalBounds.Second = { anchor.x + size.x + 1, anchor.y + size.y + 1 };
}
