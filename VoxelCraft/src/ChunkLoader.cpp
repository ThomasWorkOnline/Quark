#include "ChunkLoader.h"

#define CHUNK_UUID(coord) *reinterpret_cast<size_t*>(&coord)

Quark::Scope<ChunkLoader> ChunkLoader::Create(World* world)
{
	return Quark::CreateScope<ChunkLoader>(world);
}

static constexpr int32_t s_QueueLimit = 10000;

static std::mutex s_Mutex;
ChunkLoader::ChunkLoader(World* world)
	: m_World(world)
{
}

ChunkLoader::~ChunkLoader()
{
	/*for (auto& chunk : m_FinishedChunks)
	{
		delete chunk.second;
	}*/
}

void ChunkLoader::Queue(glm::ivec2 coord)
{
	std::lock_guard<std::mutex> lock(s_Mutex);

	if (m_LoadingQueue.size() < s_QueueLimit)
	{
		// TODO: check if chunk is already in the queue before adding it

		m_LoadingQueue[CHUNK_UUID(coord)] = std::async(std::launch::async, &ChunkLoader::OnLoad, this, coord);
	}
}

void ChunkLoader::Dispose(glm::ivec2 coord)
{
	std::lock_guard<std::mutex> lock(s_Mutex);

	if (m_LoadingQueue.size() < s_QueueLimit)
	{
		m_UnloadingQueue[CHUNK_UUID(coord)] = std::async(std::launch::async, &ChunkLoader::OnUnload, this, coord);
	}
}

bool ChunkLoader::Idling() const
{
	return !m_LoadingQueue.size() && !m_UnloadingQueue.size();
}

const std::unordered_map<size_t, Chunk*>& ChunkLoader::GetLoadedChunks() const
{
	return m_FinishedChunks;
}

Chunk* ChunkLoader::GetLoadedChunk(glm::ivec2 coord)
{
	return m_FinishedChunks[CHUNK_UUID(coord)];
}

Chunk* ChunkLoader::GetDataGeneratedChunk(glm::ivec2 coord)
{
	return m_DataGeneratedChunks[CHUNK_UUID(coord)];
}

static std::ofstream s_Debug("chunk.txt", std::ios::out);

void ChunkLoader::OnLoad(glm::ivec2 coord)
{
	UniqueChunkMeshGenerator(coord);
}

void ChunkLoader::OnUnload(glm::ivec2 coord)
{
	
}

static std::mutex s_ChunkAllocatorMutex;
Chunk* ChunkLoader::UniqueChunkAllocator(glm::ivec2 coord)
{
	std::lock_guard<std::mutex> lock(s_ChunkAllocatorMutex);

	auto ptr = m_AllocatedChunks[CHUNK_UUID(coord)];

	if (!ptr)
	{
		ptr = new Chunk(coord, this->m_World);
		m_Stats.ChunksAllocated++;

		m_AllocatedChunks[CHUNK_UUID(coord)] = ptr;
	}

	QK_ASSERT(ptr, "Unique chunk allocator returned nullptr!");

	return ptr;
}

static std::mutex s_ChunkDataGeneratorMutex;
Chunk* ChunkLoader::UniqueChunkDataGenerator(glm::ivec2 coord)
{
	std::lock_guard<std::mutex> lock(s_ChunkDataGeneratorMutex);

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

static std::mutex s_ChunkMeshGeneratorMutex;
Chunk* ChunkLoader::UniqueChunkMeshGenerator(glm::ivec2 coord)
{
	std::lock_guard<std::mutex> lock(s_ChunkMeshGeneratorMutex);

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
}
