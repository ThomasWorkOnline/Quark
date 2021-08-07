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
	static bool s_Stopping = false;
	static std::mutex s_ConditionMutex;

	static std::recursive_mutex s_LoadingQueueMutex;
	static std::recursive_mutex s_UnloadingQueueMutex;
	static std::mutex s_DataGeneratorMutex;
	static std::mutex s_MeshGeneratorMutex;

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

		m_World.Map.Foreach([](const Quark::Ref<Chunk>& data)
			{
				if (data && data->LoadStatus == Chunk::LoadStatus::Loaded)
				{
					data->UploadMesh();
				}
			});
	}

	void ChunkLoader::Load(ChunkIdentifier id)
	{
		// If chunk is queued to be unloaded, cancel unloading
		{
			std::lock_guard<std::recursive_mutex> lock(s_UnloadingQueueMutex);
			auto it = m_UnloadingQueue.find(id.ID);
			if (it != m_UnloadingQueue.end())
				m_UnloadingQueue.erase(id.ID);
		}

		std::lock_guard<std::recursive_mutex> lock(s_LoadingQueueMutex);
		if (m_LoadingQueue.size() < s_QueueLimit)
		{
			const auto& chunk = m_World.Map.Select(id);
			if ((!chunk || chunk->LoadStatus == Chunk::LoadStatus::Allocated || chunk->LoadStatus == Chunk::LoadStatus::WorldGenerated))
			{
				m_LoadingQueue.insert(id.ID);
				s_ConditionVariable.notify_one();
			}
		}
	}

	void ChunkLoader::Unload(ChunkIdentifier id)
	{
		{
			// If chunk is queued to be loaded, cancel loading
			std::lock_guard<std::recursive_mutex> lock(s_LoadingQueueMutex);
			auto it = m_LoadingQueue.find(id.ID);
			if (it != m_LoadingQueue.end())
			{
				m_LoadingQueue.erase(id.ID);
			}
		}

		std::lock_guard<std::recursive_mutex> lock(s_UnloadingQueueMutex);
		m_UnloadingQueue.insert(id.ID);
		s_ConditionVariable.notify_one();
	}

	bool ChunkLoader::Idling() const
	{
		bool loadingDone;
		{
			std::lock_guard<std::recursive_mutex> lock(s_LoadingQueueMutex);
			loadingDone = m_LoadingQueue.empty();
		}
		bool unloadingDone;
		{
			std::lock_guard<std::recursive_mutex> lock(s_UnloadingQueueMutex);
			unloadingDone = m_UnloadingQueue.empty();
		}
		return loadingDone && unloadingDone;
	}

	void ChunkLoader::ProcessLoading()
	{
		std::unique_lock<std::recursive_mutex> lock(s_LoadingQueueMutex);
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
		std::unique_lock<std::recursive_mutex> lock(s_UnloadingQueueMutex);
		for (auto id : m_UnloadingQueue)
		{
			UnloadChunk(id);
		}

		m_UnloadingQueue.clear();
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

		auto chunk = m_World.Map.Load(id);
		auto neighbors = chunk->QueryNeighbors();

		UniqueChunkDataGenerator(chunk);
		UniqueChunkDataGenerator(neighbors.North);
		UniqueChunkDataGenerator(neighbors.South);
		UniqueChunkDataGenerator(neighbors.West);
		UniqueChunkDataGenerator(neighbors.East);

		UniqueChunkMeshGenerator(chunk, neighbors);
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

	void ChunkLoader::UniqueChunkDataGenerator(const Quark::Ref<Chunk>& chunk)
	{
		bool access = false; // Thread safe, function scope access flag
		{
			std::lock_guard<std::mutex> lock(s_DataGeneratorMutex);
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

	void ChunkLoader::UniqueChunkMeshGenerator(const Quark::Ref<Chunk>& chunk, const ChunkNeighbors& neighbors)
	{
		bool access = false;
		{
			std::lock_guard<std::mutex> lock(s_MeshGeneratorMutex);
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
