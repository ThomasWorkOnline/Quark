#include "ChunkLoader.h"

Quark::Scope<ChunkLoader> ChunkLoader::Create(World* world)
{
	return Quark::CreateScope<ChunkLoader>(world);
}

static constexpr int32_t s_QueueLimit = 10000;

ChunkLoader::~ChunkLoader()
{
	// delete all chunks
	for (auto chunk : m_LoadedChunks)
	{
		delete chunk.second;
	}
}

static std::mutex s_ExternalMutex;
void ChunkLoader::Queue(glm::ivec2 coord)
{
	// lock
	std::lock_guard<std::mutex> lock(s_ExternalMutex);

	if (m_LoadingQueue.size() < s_QueueLimit)
	{
		m_LoadingQueue.push(std::async(std::launch::async, &ChunkLoader::OnLoad, this, coord));
	}
}

void ChunkLoader::Dispose(glm::ivec2 coord)
{
	// lock
	std::lock_guard<std::mutex> lock(s_ExternalMutex);

	if (m_LoadingQueue.size() < s_QueueLimit)
	{
		m_UnloadingQueue.push(std::async(std::launch::async, &ChunkLoader::OnUnload, this, coord));
	}
}

const std::unordered_map<size_t, Chunk*>& ChunkLoader::GetChunks() const
{
	// lock
	std::lock_guard<std::mutex> lock(s_ExternalMutex);

	// return data
	return m_LoadedChunks;
}

Chunk* ChunkLoader::GetChunk(const glm::ivec2& coord)
{
	// lock
	std::lock_guard<std::mutex> lock(s_ExternalMutex);

	// return data
	glm::ivec2 _coord = coord;
	return m_LoadedChunks[(*reinterpret_cast<size_t*>(&_coord))];
}

static std::mutex s_InternalMutex;
void ChunkLoader::OnLoad(glm::ivec2 coord)
{
	QK_TIME_SCOPE_DEBUG(ChunkLoader::OnLoad);

	// lock
	std::lock_guard<std::mutex> lock(s_InternalMutex);

	// generate main chunk
	auto chunk = GenerateChunk(coord);

	// generate friend chunks
	GenerateChunk(coord + glm::ivec2( 1,  0));
	GenerateChunk(coord + glm::ivec2( 0,  1));
	GenerateChunk(coord + glm::ivec2(-1,  0));
	GenerateChunk(coord + glm::ivec2( 0, -1));

	// generate mesh
	if (chunk && !chunk->MeshCreated())
		chunk->GenerateMesh();

	// TODO: flush task


}

void ChunkLoader::OnUnload(glm::ivec2 coord)
{
	// lock
	std::lock_guard<std::mutex> lock(s_InternalMutex);

	// get current chunk, if exists 
	Chunk* chunk = m_LoadedChunks[(*reinterpret_cast<size_t*>(&coord))];

	// update list of loaded chunks
	m_LoadedChunks.erase(*reinterpret_cast<size_t*>(&coord));

	// TODO: flush task
		

	// delete chunk
	delete chunk;
}

Chunk* ChunkLoader::GenerateChunk(glm::ivec2 coord)
{
	// get current chunk, if exists 
	Chunk* chunk = m_LoadedChunks[*reinterpret_cast<size_t*>(&coord)];

	// create chunk, if not exists
	if (!chunk)
	{
		// create chunk
		chunk = new Chunk(coord, m_World);

		// update list of loaded chunks
		m_LoadedChunks[*reinterpret_cast<size_t*>(&coord)] = chunk;

		// generate terrain data
		chunk->GenerateTerrain();
	}

	return chunk;
}
