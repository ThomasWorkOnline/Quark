#include "WorldArea.h"

namespace VoxelCraft {

	WorldArea::WorldArea(WorldMap& map, const glm::ivec2& size, ChunkCoord anchor)
		: m_WorldPartition(map)
	{
		Invalidate(size, anchor);
	}

	bool WorldArea::InBounds(ChunkIdentifier id) const
	{
		const auto& coord = CHUNK_COORD(id);
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
				m_WorldPartition.Load(ChunkIdentifier(x, z).ID);
			}
		}
	}

	void WorldArea::UpdateOutOfBounds(const WorldAreaBounds& bounds, const WorldAreaBounds& lastBounds)
	{
		m_ChunksOutOfBounds.clear();

		std::unordered_set<ChunkID> ids;

		for (int z = bounds.First.y; z < bounds.Second.y; z++)
		{
			for (int x = bounds.First.x; x < bounds.Second.x; x++)
			{
				ids.insert(ChunkIdentifier(x, z).ID);
			}
		}

		for (int z = lastBounds.First.y; z < lastBounds.Second.y; z++)
		{
			for (int x = lastBounds.First.x; x < lastBounds.Second.x; x++)
			{
				auto id = ChunkIdentifier(x, z).ID;
				auto it = ids.find(id);
				if (it == ids.end())
				{
					m_ChunksOutOfBounds.push_back(id);
				}
			}
		}
	}

	void WorldArea::ComputeBounds(const glm::ivec2& size, ChunkCoord anchor)
	{
		m_AccessibleBounds.First = { anchor.x - size.x, anchor.y - size.y };
		m_AccessibleBounds.Second = { anchor.x + size.x, anchor.y + size.y };

		m_InternalBounds.First = { anchor.x - size.x - 1, anchor.y - size.y - 1 };
		m_InternalBounds.Second = { anchor.x + size.x + 1, anchor.y + size.y + 1 };
	}
}
