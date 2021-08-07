#include "ChunkLoader.h"

#include "World.h"

namespace VoxelCraft {

	Quark::Scope<ChunkLoader> ChunkLoader::Create(World& world, ChunkCoord coord, uint32_t renderDistance)
	{
		return Quark::CreateScope<ChunkLoader>(world, coord, renderDistance);
	}

	static const int32_t s_QueueLimit = 10000;

	//static const uint32_t s_ThreadPoolSize = std::thread::hardware_concurrency();
	static const uint32_t s_ThreadPoolSize = 1;
	static std::vector<std::thread> s_ThreadPool;
	static std::condition_variable s_ConditionVariable;
	static std::mutex s_ConditionMutex;
	static std::recursive_mutex s_QueueMutex;
	static bool s_Stopping = false;

	static bool QueueContains(const std::set<ChunkID>& set, ChunkIdentifier id)
	{
		std::lock_guard<std::recursive_mutex> lock(s_QueueMutex);

		auto it = set.find(id.ID);
		return it != set.end();
	}

	ChunkLoader::ChunkLoader(World& world, ChunkCoord coord, uint32_t renderDistance)
		: m_World(world), m_LoadingArea(m_World.Map, { renderDistance, renderDistance }, coord),
		Coord(coord),
		RenderDistance(renderDistance)
	{
		std::cout << "Platform has: " << std::thread::hardware_concurrency() << " threads.\n";
	}

	ChunkLoader::~ChunkLoader()
	{
		Stop();
	}

	void ChunkLoader::Start()
	{
		m_LoadingArea.Foreach([this](ChunkIdentifier id)
			{
				Load(id);
			});

		for (uint32_t i = 0; i < s_ThreadPoolSize; i++)
			s_ThreadPool.emplace_back(&ChunkLoader::StartWork, this);
	}

	void ChunkLoader::Stop()
	{
		{
			std::lock_guard<std::mutex> cdnLock(s_ConditionMutex);
			s_Stopping = true;
		}

		s_ConditionVariable.notify_all();

		for (auto& thread : s_ThreadPool)
			thread.join();
	}

	void ChunkLoader::OnUpdate(float elapsedTime)
	{
		static ChunkCoord lastCoord = Coord;

		if (lastCoord != Coord)
		{
			OnChunkBorderCrossed();
		}
		lastCoord = Coord;

		m_World.Map.Foreach([](Chunk* data)
			{
				if (data && data->LoadStatus == Chunk::LoadStatus::Loaded)
				{
					data->UploadMesh();
				}
			});
	}

	void ChunkLoader::Load(ChunkIdentifier id)
	{
		std::lock_guard<std::recursive_mutex> lock(s_QueueMutex);

		// If chunk is queued to be unloaded, cancel
		if (QueueContains(m_UnloadingQueue, id))
		{
			m_UnloadingQueue.erase(id.ID);
		}

		if (m_LoadingQueue.size() < s_QueueLimit)
		{
			const Chunk* chunk = m_World.Map.Select(id);
			if ((!chunk || chunk->LoadStatus == Chunk::LoadStatus::Allocated || chunk->LoadStatus == Chunk::LoadStatus::WorldGenerated))
			{
				m_LoadingQueue.insert(id.ID);
				s_ConditionVariable.notify_one();
			}
		}
	}

	void ChunkLoader::Unload(ChunkIdentifier id)
	{
		std::lock_guard<std::recursive_mutex> lock(s_QueueMutex);

		// If chunk is queued to be loaded, cancel
		if (QueueContains(m_LoadingQueue, id))
		{
			m_LoadingQueue.erase(id.ID);
		}

		if (!QueueContains(m_UnloadingQueue, id))
		{
			m_UnloadingQueue.insert(id.ID);
			s_ConditionVariable.notify_one();
		}
	}

	bool ChunkLoader::Idling() const
	{
		std::lock_guard<std::recursive_mutex> lock(s_QueueMutex);
		return m_LoadingQueue.empty() && m_UnloadingQueue.empty();
	}

	void ChunkLoader::ProcessLoading()
	{
		std::unique_lock<std::recursive_mutex> lock(s_QueueMutex);
		if (!m_LoadingQueue.empty())
		{
			ChunkID id = *m_LoadingQueue.cbegin();
			m_LoadingQueue.erase(id);

			lock.unlock();

			LoadChunk(id);
		}
	}

	void ChunkLoader::ProcessUnloading()
	{
		std::unique_lock<std::recursive_mutex> lock(s_QueueMutex);
		if (!m_UnloadingQueue.empty())
		{
			ChunkID id = *m_UnloadingQueue.cbegin();
			m_UnloadingQueue.erase(id);

			lock.unlock();

			UnloadChunk(id);
		}
	}

	void ChunkLoader::StartWork()
	{
		using namespace std::literals::chrono_literals;

		while (true)
		{
			{
				{
					std::unique_lock<std::mutex> lock(s_ConditionMutex);
					s_ConditionVariable.wait(lock, [this]() { return !m_LoadingQueue.empty() || !m_UnloadingQueue.empty() || s_Stopping; });

					if (s_Stopping) break;
				}

				ProcessLoading();
				ProcessUnloading();
			}
		}

		std::cout << "Task done! Thread 0x" << std::hex << std::this_thread::get_id() << std::dec << " exited.\n";
	}

	void ChunkLoader::LoadChunk(ChunkIdentifier id)
	{
		//QK_TIME_SCOPE_DEBUG(ChunkLoader::LoadChunk);

		auto& map = m_World.Map;
		auto chunk	= map.Load(id);
		auto north	= map.Load(id.North());
		auto south	= map.Load(id.South());
		auto west	= map.Load(id.West());
		auto east	= map.Load(id.East());

		UniqueChunkDataGenerator(chunk);
		UniqueChunkDataGenerator(north);
		UniqueChunkDataGenerator(south);
		UniqueChunkDataGenerator(west);
		UniqueChunkDataGenerator(east);

		UniqueChunkMeshGenerator(chunk, { north, south, west, east });
	}

	void ChunkLoader::UnloadChunk(ChunkIdentifier id)
	{
		m_World.Map.Unload(id);
	}

	void ChunkLoader::OnChunkBorderCrossed()
	{
		m_LoadingArea.Invalidate({ RenderDistance, RenderDistance }, Coord);

		m_LoadingArea.Foreach([this](ChunkIdentifier id)
			{
				Load(id);
			});
		m_LoadingArea.ForeachOutOfBounds([this](ChunkIdentifier id)
			{
				Unload(id);
			});
	}

	void ChunkLoader::UniqueChunkDataGenerator(Chunk* chunk)
	{
		bool access = false; // Thread safe, function scope access flag

		{
			std::lock_guard<std::recursive_mutex> lock(s_QueueMutex);

			if (chunk && chunk->LoadStatus == Chunk::LoadStatus::Allocated)
			{
				access = true;
				chunk->LoadStatus = Chunk::LoadStatus::WorldGenerating;
			}
		}

		if (access)
		{
			chunk->BuildTerrain();
			chunk->LoadStatus = Chunk::LoadStatus::WorldGenerated;
			Stats.ChunksWorldGen++;
		}
	}

	void ChunkLoader::UniqueChunkMeshGenerator(Chunk* chunk, const ChunkNeighbors& neighbors)
	{
		bool access = false;

		{
			std::lock_guard<std::recursive_mutex> lock(s_QueueMutex);

			if (chunk && chunk->LoadStatus == Chunk::LoadStatus::WorldGenerated)
			{
				access = true;
				chunk->LoadStatus = Chunk::LoadStatus::Loading;
			}
		}

		if (access)
		{
			chunk->BuildMesh(neighbors);
			chunk->LoadStatus = Chunk::LoadStatus::Loaded;
			Stats.ChunksMeshGen++;

			m_World.OnChunkLoaded(chunk->ID);
		}
	}
}
