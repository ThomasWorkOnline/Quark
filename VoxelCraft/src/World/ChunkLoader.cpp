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
		: m_World(world), m_LoadingArea(m_World.GetMap(), { renderDistance, renderDistance }, coord),
		m_Coord(coord),
		m_RenderDistance(renderDistance)
	{
		m_LoadingArea.Foreach([this](ChunkIdentifier id)
			{
				Load(id);
			});

		for (uint32_t i = 0; i < s_ThreadPoolSize; i++)
			s_ThreadPool.emplace_back(&ChunkLoader::StartWork, this);

		std::cout << "Platform has: " << std::thread::hardware_concurrency() << " threads.\n";
	}

	ChunkLoader::~ChunkLoader()
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
		static ChunkCoord lastCoord = m_Coord;

		if (lastCoord != m_Coord)
		{
			OnChunkBorderCrossed();
		}
		lastCoord = m_Coord;

		m_World.GetMap().Foreach([](Chunk* data)
			{
				if (data && data->GetLoadStatus() == Chunk::LoadStatus::Loaded)
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
			const Chunk* chunk = m_World.GetMap().Select(id);
			if ((!chunk || chunk->GetLoadStatus() == Chunk::LoadStatus::Allocated || chunk->GetLoadStatus() == Chunk::LoadStatus::WorldGenerated))
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

	void ChunkLoader::Rebuild(ChunkIdentifier id)
	{
		Chunk* chunk = m_World.GetMap().Select(id);
		chunk->SetLoadStatus(Chunk::LoadStatus::WorldGenerated);
		LoadChunk(id);
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
		QK_TIME_SCOPE_DEBUG(ChunkLoader::LoadChunk);

		auto& map = m_World.GetMap();
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
		m_World.GetMap().Unload(id);
	}

	void ChunkLoader::OnChunkBorderCrossed()
	{
		m_LoadingArea.Invalidate({ m_RenderDistance, m_RenderDistance }, m_Coord);

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

			if (chunk && chunk->GetLoadStatus() == Chunk::LoadStatus::Allocated)
			{
				access = true;
				chunk->SetLoadStatus(Chunk::LoadStatus::WorldGenerating);
			}
		}

		if (access)
		{
			chunk->BuildTerrain();
			chunk->SetLoadStatus(Chunk::LoadStatus::WorldGenerated);
			m_Stats.ChunksWorldGen++;
		}
	}

	void ChunkLoader::UniqueChunkMeshGenerator(Chunk* chunk, const ChunkNeighbors& neighbors)
	{
		bool access = false;

		{
			std::lock_guard<std::recursive_mutex> lock(s_QueueMutex);

			if (chunk && chunk->GetLoadStatus() == Chunk::LoadStatus::WorldGenerated)
			{
				access = true;
				chunk->SetLoadStatus(Chunk::LoadStatus::Loading);
			}
		}

		if (access)
		{
			chunk->BuildMesh(neighbors);
			chunk->SetLoadStatus(Chunk::LoadStatus::Loaded);
			m_Stats.ChunksMeshGen++;

			m_World.OnChunkLoaded(chunk->GetID());
		}
	}
}
