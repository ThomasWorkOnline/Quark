#include "ChunkLoader.h"

#define CHUNK_UUID(coord) *reinterpret_cast<size_t*>(&coord)
#define CHUNK_COORD(id) *reinterpret_cast<glm::ivec2*>(&id)

Quark::Scope<ChunkLoader> ChunkLoader::Create(World* world)
{
	return Quark::CreateScope<ChunkLoader>(world);
}

static const int32_t s_QueueLimit = 10000;

static std::condition_variable s_ConditionVariable;
static std::thread s_Worker;
static std::mutex s_Mutex;
static bool s_Paused = true;
static bool s_Stopping = false;

ChunkLoader::ChunkLoader(World* world)
	: m_World(world)
{
	s_Worker = std::thread(&ChunkLoader::FlushQueue, this);

	std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

ChunkLoader::~ChunkLoader()
{
	{
		std::lock_guard<std::mutex> lock(s_Mutex);

		s_Paused = false;
		s_Stopping = true;
	}
	
	s_ConditionVariable.notify_one();

	s_Worker.join();

	while (!m_LoadingQueue.empty())
	{
		m_LoadingQueue.pop();
	}

	for (auto& chunk : m_Chunks)
	{
		delete chunk.second;
		m_Stats.ChunksFreed++;
	}

	m_Chunks.clear();

	QK_ASSERT(m_Stats.ChunksAllocated == m_Stats.ChunksFreed, "Memory leak detected!" << m_Stats.ChunksAllocated - m_Stats.ChunksFreed << " chunks could not be deleted.");


	std::cout << "Batch summary:\n" << std::dec <<
		"Allocated: " << m_Stats.ChunksAllocated << '\n' <<
		"Freed: " << m_Stats.ChunksFreed << '\n' <<
		"World Gen: " << m_Stats.ChunksWorldGen << '\n' <<
		"Mesh Gen: " << m_Stats.ChunksMeshGen << '\n';
}

void ChunkLoader::Persist(glm::ivec2 coord)
{
	std::lock_guard<std::mutex> lock(s_Mutex);

	if (m_LoadingQueue.size() < s_QueueLimit)
	{
		m_LoadingQueue.push(CHUNK_UUID(coord));

		// resume the worker
		s_Paused = false;

		s_ConditionVariable.notify_one();
	}
	else
	{
		QK_CORE_WARN("Chunk at " << coord << " refused to persist due to insufficient queue capacity.");
	}
}

void ChunkLoader::Dispose(glm::ivec2 coord)
{
	/*std::lock_guard<std::mutex> lock(s_Mutex);

	if (m_LoadingQueue.size() < s_QueueLimit)
	{
		m_UnloadingQueue[CHUNK_UUID(coord)] = std::async(std::launch::async, &ChunkLoader::Unload, this, coord);
	}*/
}

bool ChunkLoader::Idling() const
{
	std::lock_guard<std::mutex> lock(s_Mutex);

	return s_Paused;
}

const std::unordered_map<size_t, LoadingChunk*>& ChunkLoader::GetChunks() const
{
	std::lock_guard<std::mutex> lock(s_Mutex);

	return m_Chunks;
}

Chunk* ChunkLoader::GetChunk(glm::ivec2 coord)
{
	std::lock_guard<std::mutex> lock(s_Mutex);

	auto it = m_Chunks.find(CHUNK_UUID(coord));

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
		LoadingChunk* chunk = nullptr;
		{
			std::unique_lock<std::mutex> lock(s_Mutex);

			if (s_Stopping) break;

			if (m_LoadingQueue.empty())
			{
				s_Paused = true;

				// wait for the main thread to resume this
				s_ConditionVariable.wait(lock, []() { return !s_Paused; });
			}
			else
			{
				size_t id = m_LoadingQueue.front();
				auto it = m_Chunks.find(id);
				m_LoadingQueue.pop();

				// generate if not found
				if (it == m_Chunks.end())
				{
					chunk = new LoadingChunk(CHUNK_COORD(id), this->m_World);
					chunk->SetStatus(LoadingChunk::Status::Allocated);
					m_Stats.ChunksAllocated++;

					m_Chunks.insert({ id, chunk });
				}
			}
		}

		if (chunk)
		{
			chunk->GenerateWorld();
			chunk->SetStatus(LoadingChunk::Status::WorldGenerated);
			m_Stats.ChunksWorldGen++;

			chunk->GenerateMesh();
			chunk->SetStatus(LoadingChunk::Status::Loaded);
			m_Stats.ChunksMeshGen++;
		}
	}

	std::cout << "Task done! Thread 0x" << std::hex << std::this_thread::get_id() << " exited.\n";
}

/*static std::mutex s_ChunkMutex;
Chunk* ChunkLoader::UniqueChunkAllocator(glm::ivec2 coord)
{
	std::lock_guard<std::mutex> lock(s_ChunkMutex);

	// This will return nullptr if no chunk exsists
	auto ptr = m_AllocatedChunks[CHUNK_UUID(coord)];

	if (!ptr)
	{
		// Request allocation
		ptr = new Chunk(coord, this->m_World);
		m_Stats.ChunksAllocated++;

		m_AllocatedChunks[CHUNK_UUID(coord)] = ptr;
	}

	QK_ASSERT(ptr, "Unique chunk allocator returned nullptr!");

	return ptr;
}

Chunk* ChunkLoader::UniqueChunkDataGenerator(glm::ivec2 coord)
{
	std::lock_guard<std::mutex> lock(s_ChunkMutex);

	// step 1: if not allocated, allocate
	auto ptr = UniqueChunkAllocator(coord);

	// step 2: if terrain not generated, generate terrain
	auto generatedPtr = m_DataGeneratedChunks[CHUNK_UUID(coord)];

	if (!generatedPtr)
	{
		ptr->GenerateTerrain();
		m_Stats.ChunksTerrainGenerated++;

		m_DataGeneratedChunks[CHUNK_UUID(coord)] = ptr;
	}

	QK_ASSERT(ptr, "Unique chunk data generator returned nullptr!");

	return ptr;
}

Chunk* ChunkLoader::UniqueChunkMeshGenerator(glm::ivec2 coord)
{
	std::lock_guard<std::mutex> lock(s_ChunkMutex);

	// step 1: if not data generated, generate data
	auto center = UniqueChunkDataGenerator(coord);
	auto left	= UniqueChunkDataGenerator(coord + glm::ivec2(-1,  0));
	auto right	= UniqueChunkDataGenerator(coord + glm::ivec2( 1,  0));
	auto bottom = UniqueChunkDataGenerator(coord + glm::ivec2( 0, -1));
	auto top	= UniqueChunkDataGenerator(coord + glm::ivec2( 0,  1));

	// step 2: if mesh not generated, generate mesh
	auto generatedPtr = m_MeshGeneratedChunks[CHUNK_UUID(coord)];

	if (!generatedPtr)
	{
		center->GenerateMesh();
		m_Stats.ChunksMeshGenerated++;
		
		m_MeshGeneratedChunks[CHUNK_UUID(coord)] = center;

		// TODO: change
		m_FinishedChunks[CHUNK_UUID(coord)] = center;
	}

	QK_ASSERT(center, "Unique chunk mesh generator returned nullptr!");

	return center;
}*/
