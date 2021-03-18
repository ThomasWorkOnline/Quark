#include "World.h"

#include <future>
#include <chrono>

static std::atomic<bool> s_WorldGenerating = true;
static std::atomic<uint32_t> s_ChunksInitialized = 0;

static std::mutex s_ChunkMutex;
static void ConstructChunkData(Chunk* chunk)
{
	std::lock_guard<std::mutex> lock(s_ChunkMutex);
	chunk->ConstructData(s_WorldGenerating);
	s_ChunksInitialized++;
}

static void ConstructChunkMesh(Chunk* chunk, World* world)
{
	while (s_ChunksInitialized != world->GetChunks().size());

	std::lock_guard<std::mutex> lock(s_ChunkMutex);
	chunk->ConstructMesh(s_WorldGenerating);
}

void World::InitializeChunksAsync()
{
	NT_TIME_SCOPE_DEBUG(World::InitializeChunksAsync);

	m_Chunks.reserve((size_t)m_RenderDistance * (size_t)m_RenderDistance);

	for (int32_t y = 0; y < m_RenderDistance; y++)
	{
		for (int32_t x = 0; x < m_RenderDistance; x++)
		{
			glm::ivec2 position = { x - m_RenderDistance / 2, y };
			m_Chunks.emplace_back(position, this);
		}
	}

	for (auto& chunk : m_Chunks)
		m_ChunksConstructDataFutures.push_back(std::async(std::launch::async, ConstructChunkData, &chunk));

	for (auto& chunk : m_Chunks)
		m_ChunksConstructMeshFutures.push_back(std::async(std::launch::async, ConstructChunkMesh, &chunk, this));
}

const Chunk* World::GetChunk(const glm::ivec2& position) const
{
	for (auto& chunk : m_Chunks)
	{
		if (chunk.GetPosition() == position)
		{
			return &chunk;
		}
	}
	return nullptr;
}

void World::Shutdown()
{
	s_WorldGenerating = false;
}

World::World()
{
	InitializeChunksAsync();
}

World::~World()
{

}
