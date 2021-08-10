#include "ChunkLoader.h"

#include "World.h"

namespace VoxelCraft {

	Quark::Scope<ChunkLoader> ChunkLoader::Create(World* world, ChunkCoord coord, uint32_t renderDistance)
	{
		return Quark::CreateScope<ChunkLoader>(world, coord, renderDistance);
	}

	static Quark::Timer s_Timer;
	static bool s_Working = false;

	static constexpr int32_t s_QueueLimit = 10000;

	//static const uint32_t s_ThreadPoolSize = std::thread::hardware_concurrency();
	static const uint32_t s_ThreadPoolSize = 1;
	static std::vector<std::thread> s_ThreadPool;

	static std::condition_variable s_ConditionVariable;
	static bool s_Stopping = false;
	static std::mutex s_ConditionMutex;

	static std::recursive_mutex s_LoadingQueueMutex;
	static std::recursive_mutex s_UnloadingQueueMutex;

	ChunkLoader::ChunkLoader(World* world, ChunkCoord coord, uint32_t renderDistance)
		: m_World(world), m_LoadingArea(m_World->Map, { renderDistance, renderDistance }, coord),
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
			s_ThreadPool.emplace_back(&ChunkLoader::Work, this);
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

	void ChunkLoader::Invalidate()
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
			const auto& chunk = m_World->Map.Get(id);
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

	void ChunkLoader::Work()
	{
		using namespace std::literals::chrono_literals;

		OnResume();
		while (!s_Stopping)
		{
			{
				std::unique_lock<std::mutex> lock(s_ConditionMutex);
				s_ConditionVariable.wait(lock, [this]()
					{
						bool idle = Idling();
						if (idle)
							OnIdle();
						return !idle || s_Stopping;
					});

				if (!s_Working)
				{
					OnResume();
				}
			}

			ProcessLoading();
			ProcessUnloading();
		}

		std::cout << "Task done! Thread 0x" << std::hex << std::this_thread::get_id() << std::dec << " exited.\n";
	}

	void ChunkLoader::ProcessLoading()
	{
		std::unique_lock<std::recursive_mutex> lock(s_LoadingQueueMutex);
		if (!m_LoadingQueue.empty())
		{
			ChunkID id = *m_LoadingQueue.cbegin();
			m_LoadingQueue.erase(id);
			lock.unlock();

			m_World->Map.Load(id);
			m_World->OnChunkLoaded(id);
		}
	}

	void ChunkLoader::ProcessUnloading()
	{
		std::unique_lock<std::recursive_mutex> lock(s_UnloadingQueueMutex);
		for (auto id : m_UnloadingQueue)
		{
			m_World->Map.Unload(id);
		}

		m_UnloadingQueue.clear();
	}

	void ChunkLoader::OnIdle()
	{
		s_Working = false;
		s_Timer.Stop();
		std::cout << "Map loading took: " << s_Timer.Milliseconds() << "ms\n";
	}

	void ChunkLoader::OnResume()
	{
		s_Working = true;
		s_Timer.Start();
	}
}
