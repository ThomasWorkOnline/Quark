#include "ChunkLoader.h"

Quark::Scope<ChunkLoader> ChunkLoader::Create(World* world, const glm::ivec2& coord, uint32_t renderDistance)
{
	return Quark::CreateScope<ChunkLoader>(world, coord, renderDistance);
}

static const int32_t s_QueueLimit = 10000;

static std::thread s_Worker;
static std::condition_variable s_ConditionVariable;
static std::mutex s_ConditionMutex;
static bool s_Paused = true;
static bool s_Stopping = false;

static std::recursive_mutex s_Mutex;

static bool QueueContains(const std::list<size_t>& list, size_t id)
{
	std::lock_guard<std::recursive_mutex> lock(s_Mutex);

	for (size_t _id : list)
	{
		if (_id == id)
			return true;
	}

	return false;
}

static bool QueueContains(const std::list<size_t>& list, glm::ivec2 coord)
{
	return QueueContains(list, CHUNK_UUID(coord));
}

ChunkLoader::ChunkLoader(World* world, const glm::ivec2& coord, uint32_t renderDistance)
	: m_LoadingArea(world->m_Map, { renderDistance, renderDistance }, coord),
	m_Coord(coord),
	m_RenderDistance(renderDistance),
	m_World(world)
{
	s_Worker = std::thread(&ChunkLoader::FlushQueue, this);

	m_LoadingArea.Foreach([this](glm::ivec2 coord)
		{
			Load(coord);
		});
}

ChunkLoader::~ChunkLoader()
{
	{
		std::lock_guard<std::mutex> cdnLock(s_ConditionMutex);

		s_Paused = false;
		s_Stopping = true;
	}
	
	s_ConditionVariable.notify_all();

	s_Worker.join();

	QK_ASSERT(m_Stats.ChunksAllocated == m_Stats.ChunksFreed, "Memory leak detected!" << m_Stats.ChunksAllocated - m_Stats.ChunksFreed << " chunks could not be deleted.");

	std::cout << "Batch summary:\n" <<
		"Allocated: " << m_Stats.ChunksAllocated << '\n' <<
		"Freed: " << m_Stats.ChunksFreed << '\n' <<
		"World Gen: " << m_Stats.ChunksWorldGen << '\n' <<
		"Mesh Gen: " << m_Stats.ChunksMeshGen << '\n';
}

void ChunkLoader::OnUpdate(float elapsedTime)
{
	static glm::ivec2 lastCoord = m_Coord;

	if (lastCoord != m_Coord)
	{
		OnChunkBorderCrossed();
	}
	lastCoord = m_Coord;

	m_World->m_Map.Foreach([](Chunk* data)
		{
			if (data && data->GetLoadStatus() == Chunk::LoadStatus::Loaded
				&& !data->m_Pushed)
			{
				data->PushData();
			}
		});
}

void ChunkLoader::Load(glm::ivec2 coord)
{
	std::lock_guard<std::recursive_mutex> lock(s_Mutex);

	if (m_LoadingQueue.size() < s_QueueLimit)
	{
		Chunk* chunk = m_World->m_Map.Select(coord);
		if ((!chunk || chunk->GetLoadStatus() == Chunk::LoadStatus::Allocated || chunk->GetLoadStatus() == Chunk::LoadStatus::WorldGenerated) && !QueueContains(m_LoadingQueue, coord))
		{
			m_LoadingQueue.push_back(CHUNK_UUID(coord));

			std::unique_lock<std::mutex> cdnLock(s_ConditionMutex);

			s_Paused = false;
			cdnLock.unlock();

			s_ConditionVariable.notify_one();
		}
	}
}

void ChunkLoader::Unload(glm::ivec2 coord)
{
	std::lock_guard<std::recursive_mutex> lock(s_Mutex);

	Chunk* chunk = m_World->m_Map.Select(coord);
	if (chunk && !QueueContains(m_UnloadingQueue, coord))
	{
		m_UnloadingQueue.push_back(CHUNK_UUID(coord));

		std::unique_lock<std::mutex> cdnLock(s_ConditionMutex);

		s_Paused = false;
		cdnLock.unlock();

		s_ConditionVariable.notify_one();
	}
}

void ChunkLoader::Rebuild(glm::ivec2 coord)
{
	auto chunk = m_World->m_Map.Select(coord);
	chunk->SetLoadStatus(Chunk::LoadStatus::WorldGenerated);
	LoadChunk(CHUNK_UUID(coord));
}

bool ChunkLoader::Idling() const
{
	std::lock_guard<std::mutex> cdnLock(s_ConditionMutex);

	return s_Paused;
}

void ChunkLoader::FlushQueue()
{
	using namespace std::literals::chrono_literals;

	while (true)
	{
		{
			{
				std::unique_lock<std::mutex> cdnLock(s_ConditionMutex);

				if (m_LoadingQueue.empty() && m_UnloadingQueue.empty())
				{
					s_Paused = true;
				}

				s_ConditionVariable.wait(cdnLock, []() { return !s_Paused; });

				if (s_Stopping) break;
			}

			{
				std::unique_lock<std::recursive_mutex> lock(s_Mutex);
				if (!m_LoadingQueue.empty())
				{
					size_t id = m_LoadingQueue.front();
					m_LoadingQueue.pop_front();

					lock.unlock();

					LoadChunk(id);
				}
			}
			
			{
				std::unique_lock<std::recursive_mutex> lock(s_Mutex);
				if (!m_UnloadingQueue.empty())
				{
					size_t id = m_UnloadingQueue.front();
					m_UnloadingQueue.pop_front();

					lock.unlock();

					UnloadChunk(id);
				}
			}
		}
	}

	std::cout << "Task done! Thread 0x" << std::hex << std::this_thread::get_id() << std::dec << " exited.\n";
}

void ChunkLoader::OnChunkBorderCrossed()
{
	m_LoadingArea.Invalidate({ m_RenderDistance, m_RenderDistance }, m_Coord);
	m_LoadingArea.Foreach([this](glm::ivec2 coord)
		{
			Load(coord);
		});
	m_LoadingArea.OnUnload([this](glm::ivec2 coord)
		{
			Unload(coord);
		});
}

void ChunkLoader::LoadChunk(size_t id)
{
	QK_TIME_SCOPE_DEBUG(ChunkLoader::LoadChunk);

	glm::ivec2 coord = CHUNK_COORD(id);
	auto chunk = m_World->m_Map.Load(id);
	auto left  = m_World->m_Map.Load(coord + glm::ivec2(-1,  0));
	auto right = m_World->m_Map.Load(coord + glm::ivec2( 1,  0));
	auto back  = m_World->m_Map.Load(coord + glm::ivec2( 0, -1));
	auto front = m_World->m_Map.Load(coord + glm::ivec2( 0,  1));

	UniqueChunkDataGenerator(chunk);
	UniqueChunkDataGenerator(left);
	UniqueChunkDataGenerator(right);
	UniqueChunkDataGenerator(back);
	UniqueChunkDataGenerator(front);

	UniqueChunkMeshGenerator(chunk, left, right, back, front);
}

void ChunkLoader::UnloadChunk(size_t id)
{
	m_World->m_Map.Unload(id);
}

void ChunkLoader::UniqueChunkDataGenerator(Chunk* chunk)
{
	{
		std::lock_guard<std::recursive_mutex> lock(s_Mutex);

		if (chunk && chunk->GetLoadStatus() == Chunk::LoadStatus::Allocated)
		{
			chunk->SetLoadStatus(Chunk::LoadStatus::WorldGenerating);
		}
	}

	if (chunk && chunk->GetLoadStatus() == Chunk::LoadStatus::WorldGenerating)
	{
		chunk->GenerateWorld();
		chunk->SetLoadStatus(Chunk::LoadStatus::WorldGenerated);
		m_Stats.ChunksWorldGen++;
	}
}

void ChunkLoader::UniqueChunkMeshGenerator(Chunk* chunk, Chunk* left, Chunk* right, Chunk* back, Chunk* front)
{
	{
		std::lock_guard<std::recursive_mutex> lock(s_Mutex);

		if (chunk && chunk->GetLoadStatus() == Chunk::LoadStatus::WorldGenerated)
		{
			chunk->SetLoadStatus(Chunk::LoadStatus::Loading);
		}
	}

	if (chunk && chunk->GetLoadStatus() == Chunk::LoadStatus::Loading)
	{
		chunk->GenerateMesh(left, right, back, front);
		chunk->SetLoadStatus(Chunk::LoadStatus::Loaded);
		m_Stats.ChunksMeshGen++;
	}
}
