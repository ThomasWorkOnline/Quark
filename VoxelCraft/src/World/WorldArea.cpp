#include "WorldArea.h"

WorldArea::WorldArea(WorldMap& map, const glm::ivec2& size, const glm::ivec2& anchor)
	: m_WorldPartition(map)
{
	Invalidate(size, anchor);
}

bool WorldArea::InBounds(size_t id) const
{
	const auto& coord = CHUNK_COORD(id);
	return (coord.x >= m_AccessibleBounds.First.x && coord.x <= m_AccessibleBounds.Second.x &&
		coord.y >= m_AccessibleBounds.First.y && coord.y <= m_AccessibleBounds.Second.y);
}

void WorldArea::Foreach(const std::function<void(size_t id)>& func) const
{
	for (int z = m_AccessibleBounds.First.y; z < m_AccessibleBounds.Second.y; z++)
	{
		for (int x = m_AccessibleBounds.First.x; x < m_AccessibleBounds.Second.x; x++)
		{
			glm::ivec2 coord(x, z);
			func(CHUNK_UUID(coord));
		}
	}
}

void WorldArea::Invalidate(const glm::ivec2& size, const glm::ivec2& anchor)
{
	static WorldAreaBounds lastBounds = m_InternalBounds;

	ComputeBounds(size, anchor);
	LoadArea();
	UpdateOutOfBounds(m_InternalBounds, lastBounds);

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

void WorldArea::UpdateOutOfBounds(const WorldAreaBounds& bounds, const WorldAreaBounds& lastBounds)
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
				m_WorldPartition.Unload(CHUNK_UUID(coord));
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
