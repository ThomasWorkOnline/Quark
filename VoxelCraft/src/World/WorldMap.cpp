#include "WorldMap.h"

#include "World.h"
#include "ChunkFactory.h"

#include <queue>

namespace VoxelCraft {

	static size_t s_MaxBucketSize = 0;

	WorldMap::WorldMap(World* world)
		: m_World(world) {}

	void WorldMap::OnUpdate(float elapsedTime)
	{
		std::lock_guard<std::mutex> lock(m_ChunksToDeleteMutex);
		for (auto& chunk : m_ChunksToDelete)
		{
			chunk.reset();
		}
	}

	void WorldMap::Foreach(const std::function<void(ChunkIdentifier id)>& func) const
	{
		std::queue<ChunkID> queue;
		{
			std::lock_guard<std::mutex> lock(m_ChunksLocationsMutex);
			for (auto& e : m_ChunksLocations)
			{
				queue.push(e.first);
			}
		}

		while (!queue.empty())
		{
			func(queue.front());
			queue.pop();
		}
	}

	void WorldMap::Foreach(const std::function<void(const Quark::Ref<Chunk>& data)>& func) const
	{
		std::queue<Quark::Ref<Chunk>> queue;
		{
			std::lock_guard<std::mutex> lock(m_ChunksLocationsMutex);
			for (auto& e : m_ChunksLocations)
			{
				queue.push(e.second);
			}
		}
		
		while (!queue.empty())
		{
			func(queue.front());
			queue.pop();
		}
	}

	size_t WorldMap::Count() const
	{
		std::lock_guard<std::mutex> lock(m_ChunksLocationsMutex);
		return m_ChunksLocations.size();
	}

	size_t WorldMap::MaxBucketSize() const
	{
		std::lock_guard<std::mutex> lock(m_ChunksLocationsMutex);
		return s_MaxBucketSize;
	}

	Quark::Ref<Chunk> WorldMap::Get(ChunkIdentifier id) const
	{
		std::lock_guard<std::mutex> lock(m_ChunksLocationsMutex);
		auto it = m_ChunksLocations.find(id.ID);

		if (it != m_ChunksLocations.end())
		{
			return it->second;
		}
		return nullptr;
	}

	void WorldMap::Load(ChunkIdentifier id)
	{
		const auto& data = Create(id);
		Load(data);
	}

	void WorldMap::Load(const Quark::Ref<Chunk>& data)
	{
		const auto& neighbors = GetNonNullNeighbors(data->ID);

		ChunkFactory::BuildTerrain(data);
		ChunkFactory::BuildTerrain(neighbors.North);
		ChunkFactory::BuildTerrain(neighbors.South);
		ChunkFactory::BuildTerrain(neighbors.West);
		ChunkFactory::BuildTerrain(neighbors.East);

		ChunkFactory::BuildMesh(data, neighbors);
	}

	void WorldMap::Unload(ChunkIdentifier id)
	{
		const auto& data = Get(id);
		Unload(data);
	}

	void WorldMap::Unload(const Quark::Ref<Chunk>& data)
	{
		data->Save();

		// TODO: investigate the reason why a chunk can be null
		if (data)
			Erase(data);
	}

	bool WorldMap::Contains(ChunkIdentifier id) const
	{
		return Get(id) != nullptr;
	}

	ChunkNeighbors WorldMap::GetNonNullNeighbors(ChunkIdentifier id)
	{
		return {
			m_World->Map.Create(id.North()),
			m_World->Map.Create(id.South()),
			m_World->Map.Create(id.West()),
			m_World->Map.Create(id.East())
		};
	}

	Quark::Ref<Chunk> WorldMap::Create(ChunkIdentifier id)
	{
		// TODO: Check if the chunk is outside the writable areas ( -2 147 483 648 to +2 147 483 647 )

		auto data = Get(id);
		if (!data)
		{
			data = Quark::CreateRef<Chunk>(m_World, id);

			std::lock_guard<std::mutex> lock(m_ChunksLocationsMutex);
			m_ChunksLocations.emplace(id.ID, data);

			size_t bucket = m_ChunksLocations.bucket(id.ID);
			size_t size = m_ChunksLocations.bucket_size(bucket);
			s_MaxBucketSize = std::max(s_MaxBucketSize, size);
		}
		return data;
	}

	void WorldMap::Erase(const Quark::Ref<Chunk>& data)
	{
		{
			std::lock_guard<std::mutex> lock(m_ChunksLocationsMutex);
			m_ChunksLocations.erase(data->ID.ID);
		}

		std::lock_guard<std::mutex> lock(m_ChunksToDeleteMutex);
		m_ChunksToDelete.push_back(data);
	}
}
