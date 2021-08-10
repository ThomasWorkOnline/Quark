#include "WorldArea.h"

namespace VoxelCraft {

	WorldArea::WorldArea(WorldMap& map, const glm::ivec2& size, ChunkCoord anchor)
		: m_WorldPartition(map)
	{
		Invalidate(size, anchor);
	}

	bool WorldArea::InBounds(ChunkIdentifier id) const
	{
		const auto& coord = id.Coord;
		return (coord.x >= m_AccessibleBounds.First.x && coord.x <= m_AccessibleBounds.Second.x &&
			coord.y >= m_AccessibleBounds.First.y && coord.y <= m_AccessibleBounds.Second.y);
	}

	void WorldArea::Foreach(const std::function<void(ChunkIdentifier id)>& func) const
	{
		for (int z = m_AccessibleBounds.First.y; z < m_AccessibleBounds.Second.y; z++)
		{
			for (int x = m_AccessibleBounds.First.x; x < m_AccessibleBounds.Second.x; x++)
			{
				func(ChunkIdentifier(x, z));
			}
		}
	}

	void WorldArea::ForeachOutOfBounds(const std::function<void(ChunkIdentifier id)>& func) const
	{
		for (auto id : m_ChunksOutOfBounds)
		{
			func(id);
		}
	}

	void WorldArea::Invalidate(const glm::ivec2& size, ChunkCoord anchor)
	{
		static WorldAreaBounds lastBounds = m_InternalBounds;

		ComputeBounds(size, anchor);
		UpdateOutOfBounds(m_InternalBounds, lastBounds);

		lastBounds = m_InternalBounds;
	}

	void WorldArea::UpdateOutOfBounds(const WorldAreaBounds& bounds, const WorldAreaBounds& lastBounds)
	{
		m_ChunksOutOfBounds.clear();

		m_WorldPartition.Foreach([this](const Chunk* chunk) {
			auto id = chunk->ID;
			if (!InInternalBounds(id))
			{
				m_ChunksOutOfBounds.push_back(id);
			}
			});
	}

	bool WorldArea::InInternalBounds(ChunkIdentifier id) const
	{
		const auto& coord = id.Coord;
		return (coord.x >= m_InternalBounds.First.x && coord.x <= m_InternalBounds.Second.x &&
			coord.y >= m_InternalBounds.First.y && coord.y <= m_InternalBounds.Second.y);
	}

	void WorldArea::ComputeBounds(const glm::ivec2& size, ChunkCoord anchor)
	{
		m_AccessibleBounds.First = { anchor.x - size.x, anchor.y - size.y };
		m_AccessibleBounds.Second = { anchor.x + size.x, anchor.y + size.y };

		m_InternalBounds.First = { anchor.x - size.x - 1, anchor.y - size.y - 1 };
		m_InternalBounds.Second = { anchor.x + size.x + 1, anchor.y + size.y + 1 };
	}
}
