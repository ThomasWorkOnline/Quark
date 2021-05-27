#include "ChunkLoader.h"

Quark::Scope<ChunkLoader> ChunkLoader::Create(World* world)
{
	return Quark::CreateScope<ChunkLoader>(world);
}

static const int32_t s_QueueLimit = 10000;

static std::thread s_Worker;
static std::condition_variable s_ConditionVariable;
static std::mutex s_ConditionMutex;
static bool s_Paused = true;
static bool s_Stopping = false;

static std::recursive_mutex s_Mutex;

ChunkLoader::ChunkLoader(World* world)
	: m_World(world)
{
	s_Worker = std::thread(&ChunkLoader::FlushQueue, this);
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

	for (auto& chunk : m_Chunks)
	{
		delete chunk.second;
		m_Stats.ChunksFreed++;
	}

	QK_ASSERT(m_Stats.ChunksAllocated == m_Stats.ChunksFreed, "Memory leak detected!" << m_Stats.ChunksAllocated - m_Stats.ChunksFreed << " chunks could not be deleted.");

	std::cout << "Batch summary:\n" << std::dec <<
		"Allocated: " << m_Stats.ChunksAllocated << '\n' <<
		"Freed: " << m_Stats.ChunksFreed << '\n' <<
		"World Gen: " << m_Stats.ChunksWorldGen << '\n' <<
		"Mesh Gen: " << m_Stats.ChunksMeshGen << '\n';
}

void ChunkLoader::OnUpdate(float elapsedTime)
{
	for (auto& chunk : m_Chunks)
	{
		if (chunk.second->GetStatus() == Chunk::Status::Loaded
			&& !chunk.second->IsPushed())
		{
			chunk.second->PushData();
		}
	}
}

void ChunkLoader::Load(glm::ivec2 coord)
{
	std::lock_guard<std::recursive_mutex> lock(s_Mutex);

	if (m_LoadingQueue.size() < s_QueueLimit)
	{
		Chunk* chunk = GetChunk(coord);
		if (!QueueContains(CHUNK_UUID(coord)) && (!chunk || chunk->GetStatus() <= Chunk::Status::WorldGenerated))
		{
			m_LoadingQueue.emplace_front(CHUNK_UUID(coord));

			std::unique_lock<std::mutex> cdnLock(s_ConditionMutex);

			s_Paused = false;
			cdnLock.unlock();

			s_ConditionVariable.notify_one();
		}
	}
	else
	{
		QK_CORE_WARN("Chunk at " << coord << " refused to persist due to insufficient queue capacity.");
	}
}

void ChunkLoader::Unload(glm::ivec2 coord)
{
	Chunk* chunk = nullptr;
	{
		std::lock_guard<std::recursive_mutex> lock(s_Mutex);

		// if not already loaded, remove from loading queue
		RemoveFromQueue(CHUNK_UUID(coord));

		auto it = m_Chunks.find(CHUNK_UUID(coord));

		if (it != m_Chunks.end())
		{
			chunk = it->second;
			chunk->SetStatus(Chunk::Status::Deallocating);

			m_Chunks.erase(CHUNK_UUID(coord));
		}
	}

	if (chunk && chunk->GetStatus() == Chunk::Status::Deallocating)
	{
		delete chunk;
		m_Stats.ChunksFreed++;
	}
}

void ChunkLoader::Rebuild(Chunk* chunk, Chunk* corner1, Chunk* corner2)
{
	// Has to have context of next chunks in order to generate meshes properly
	// At most 3 chunks can regenerate when an action triggers a chunk update

	int updated = 0;

	if (chunk)
	{
		chunk->GenerateMesh();
	}

	if (corner1)
	{
		corner1->GenerateMesh();
	}

	if (corner2)
	{
		corner2->GenerateMesh();
	}
}

bool ChunkLoader::Idling() const
{
	std::lock_guard<std::mutex> cdnLock(s_ConditionMutex);

	return s_Paused;
}

const std::unordered_map<size_t, Chunk*>& ChunkLoader::GetChunks() const
{
	std::lock_guard<std::recursive_mutex> lock(s_Mutex);

	return m_Chunks;
}

Chunk* ChunkLoader::GetChunk(glm::ivec2 coord)
{
	return GetChunk(CHUNK_UUID(coord));
}

Chunk* ChunkLoader::GetChunk(size_t id)
{
	std::lock_guard<std::recursive_mutex> lock(s_Mutex);

	auto it = m_Chunks.find(id);

	if (it != m_Chunks.end())
	{
		return it->second;
	}

	return nullptr;
}

void ChunkLoader::FlushQueue()
{
	using namespace std::literals::chrono_literals;

	while (true)
	{
		Chunk* chunk = nullptr;
		{
			{
				std::unique_lock<std::mutex> cdnLock(s_ConditionMutex);

				if (m_LoadingQueue.empty())
				{
					s_Paused = true;
				}

				s_ConditionVariable.wait(cdnLock, []() { return !s_Paused; });

				if (s_Stopping) break;
			}

			std::unique_lock<std::recursive_mutex> lock(s_Mutex);

			if (!m_LoadingQueue.empty())
			{
				size_t id = m_LoadingQueue.back();
				m_LoadingQueue.pop_back();

				chunk = UniqueChunkAllocator(id);
			}
		}

		QK_TIME_SCOPE_DEBUG(Generating Chunk);

		UniqueChunkDataGenerator(chunk);
		UniqueChunkMeshGenerator(chunk);
	}

	std::cout << "Task done! Thread 0x" << std::hex << std::this_thread::get_id() << std::dec << " exited.\n";
}

bool ChunkLoader::QueueContains(size_t id)
{
	std::lock_guard<std::recursive_mutex> lock(s_Mutex);

	for (size_t _id : m_LoadingQueue)
	{
		if (_id == id)
			return true;
	}

	return false;
}

void ChunkLoader::RemoveFromQueue(size_t id)
{
	std::lock_guard<std::recursive_mutex> lock(s_Mutex);

	for (auto it = m_LoadingQueue.begin(); it != m_LoadingQueue.end(); it++)
	{
		if (*it == id)
		{
			m_LoadingQueue.erase(it);
			break;
		}
	}
}

Chunk* ChunkLoader::UniqueChunkAllocator(glm::ivec2 coord)
{
	return UniqueChunkAllocator(CHUNK_UUID(coord));
}

/// <summary>
/// Thread safe, unique chunk allocator
/// </summary>
/// <param name="id"></param>
/// <returns></returns>
Chunk* ChunkLoader::UniqueChunkAllocator(size_t id)
{
	std::lock_guard<std::recursive_mutex> lock(s_Mutex);

	Chunk* chunk = GetChunk(id);

	if (!chunk)
	{
		chunk = new Chunk(CHUNK_COORD(id), this->m_World);
		m_Stats.ChunksAllocated++;

		m_Chunks.insert({ id, chunk });
	}

	return chunk;
}

/// <summary>
/// Thread safe, unique chunk world data generator
/// </summary>
/// <param name="chunk"></param>
void ChunkLoader::UniqueChunkDataGenerator(Chunk* chunk)
{
	{
		std::lock_guard<std::recursive_mutex> lock(s_Mutex);

		if (chunk && chunk->GetStatus() == Chunk::Status::Allocated)
		{
			chunk->SetStatus(Chunk::Status::WorldGenerating);
		}
	}

	if (chunk && chunk->GetStatus() == Chunk::Status::WorldGenerating)
	{
		chunk->GenerateWorld();
		chunk->SetStatus(Chunk::Status::WorldGenerated);
		m_Stats.ChunksWorldGen++;
	}
}

/// <summary>
/// Thread safe, unique chunk mesh generator
/// </summary>
/// <param name="chunk"></param>
/// <param name="right"></param>
/// <param name="left"></param>
/// <param name="front"></param>
/// <param name="back"></param>
void ChunkLoader::UniqueChunkMeshGenerator(Chunk* chunk)
{
	{
		std::lock_guard<std::recursive_mutex> lock(s_Mutex);

		if (chunk && chunk->GetStatus() == Chunk::Status::WorldGenerated)
		{
			chunk->SetStatus(Chunk::Status::Loading);
		}
	}

	if (chunk && chunk->GetStatus() == Chunk::Status::Loading)
	{
		chunk->GenerateMesh();
		chunk->SetStatus(Chunk::Status::Loaded);
		m_Stats.ChunksMeshGen++;
	}
}
