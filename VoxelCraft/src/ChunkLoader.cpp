#include "ChunkLoader.h"

#define CHUNK_UUID(coord) *reinterpret_cast<size_t*>(&coord)
#define CHUNK_COORD(id) *reinterpret_cast<glm::ivec2*>(&id)

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
		if (chunk.second->GetStatus() == LoadingChunk::Status::Loaded
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
		if (!GetChunk(coord))
		{
			m_LoadingQueue.push(CHUNK_UUID(coord));

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
	
}

void ChunkLoader::Rebuild(Chunk* chunk)
{
	if (chunk)
	{
		glm::ivec2 coord = chunk->GetCoord();

		Chunk* nR = GetChunk(coord + glm::ivec2( 1,  0));
		Chunk* nL = GetChunk(coord + glm::ivec2(-1,  0));
		Chunk* nF = GetChunk(coord + glm::ivec2( 0,  1));
		Chunk* nB = GetChunk(coord + glm::ivec2( 0, -1));

		chunk->GenerateMesh(nR, nL, nF, nB);
		chunk->PushData();
	}
}

bool ChunkLoader::Idling() const
{
	std::lock_guard<std::mutex> cdnLock(s_ConditionMutex);

	return s_Paused;
}

const std::unordered_map<size_t, LoadingChunk*>& ChunkLoader::GetChunks() const
{
	std::lock_guard<std::recursive_mutex> lock(s_Mutex);

	return m_Chunks;
}

LoadingChunk* ChunkLoader::GetChunk(glm::ivec2 coord)
{
	return GetChunk(CHUNK_UUID(coord));
}

LoadingChunk* ChunkLoader::GetChunk(size_t id)
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
		QK_TIME_SCOPE_DEBUG(Generating Chunk);

		LoadingChunk* chunk = nullptr;
		LoadingChunk* nR = nullptr;
		LoadingChunk* nL = nullptr;
		LoadingChunk* nF = nullptr;
		LoadingChunk* nB = nullptr;
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

			std::lock_guard<std::recursive_mutex> lock(s_Mutex);

			if (!m_LoadingQueue.empty())
			{
				size_t id = m_LoadingQueue.front();
				m_LoadingQueue.pop();

				glm::ivec2 coord = CHUNK_COORD(id);
				glm::ivec2 coordNR = coord + glm::ivec2( 1,  0);
				glm::ivec2 coordNL = coord + glm::ivec2(-1,  0);
				glm::ivec2 coordNF = coord + glm::ivec2( 0,  1);
				glm::ivec2 coordNB = coord + glm::ivec2( 0, -1);

				chunk = UniqueChunkAllocator(id);
				nR = UniqueChunkAllocator(coordNR);
				nL = UniqueChunkAllocator(coordNL);
				nF = UniqueChunkAllocator(coordNF);
				nB = UniqueChunkAllocator(coordNB);
			}
		}

		UniqueChunkDataGenerator(chunk);
		UniqueChunkDataGenerator(nR);
		UniqueChunkDataGenerator(nL);
		UniqueChunkDataGenerator(nF);
		UniqueChunkDataGenerator(nB);

		chunk->GenerateMesh(nR, nL, nF, nB);
		chunk->SetStatus(LoadingChunk::Status::Loaded);
		m_Stats.ChunksMeshGen++;
	}

	std::cout << "Task done! Thread 0x" << std::hex << std::this_thread::get_id() << " exited.\n";
}

LoadingChunk* ChunkLoader::UniqueChunkAllocator(glm::ivec2 coord)
{
	return UniqueChunkAllocator(CHUNK_UUID(coord));
}

LoadingChunk* ChunkLoader::UniqueChunkAllocator(size_t id)
{
	LoadingChunk* chunk = GetChunk(id);

	if (!chunk)
	{
		chunk = new LoadingChunk(CHUNK_COORD(id), this->m_World);
		m_Stats.ChunksAllocated++;

		std::lock_guard<std::recursive_mutex> lock(s_Mutex);

		m_Chunks.insert({ id, chunk });
	}

	return chunk;
}

void ChunkLoader::UniqueChunkDataGenerator(LoadingChunk* chunk)
{
	if (chunk && chunk->GetStatus() == LoadingChunk::Status::Allocated)
	{
		chunk->GenerateWorld();
		chunk->SetStatus(LoadingChunk::Status::WorldGenerated);
		m_Stats.ChunksWorldGen++;
	}
}
