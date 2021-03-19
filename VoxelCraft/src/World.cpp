#include "World.h"

#include <future>
#include <chrono>

static std::atomic<bool> s_WorldGenerating = true;
static std::atomic<uint32_t> s_ChunksInitialized = 0;

static std::mutex s_ChunkMutex;
static void ConstructChunkData(Chunk* chunk)
{
	std::lock_guard<std::mutex> lock(s_ChunkMutex);
	chunk->ConstructChunkData(s_WorldGenerating);
	s_ChunksInitialized++;
}

static void ConstructChunkMesh(Chunk* chunk, World* world)
{
	while (s_ChunksInitialized != world->GetChunks().size());

	std::lock_guard<std::mutex> lock(s_ChunkMutex);
	chunk->ConstructChunkMesh(s_WorldGenerating);
}

void World::InitializeChunksAsync()
{
	NT_TIME_SCOPE_DEBUG(World::InitializeChunksAsync);

	m_Chunks.reserve((size_t)m_RenderDistance * (size_t)m_RenderDistance);
	m_ChunksConstructDataFutures.reserve((size_t)m_RenderDistance * (size_t)m_RenderDistance);
	m_ChunksConstructMeshFutures.reserve((size_t)m_RenderDistance * (size_t)m_RenderDistance);

	for (int32_t y = 0; y < m_RenderDistance; y++)
	{
		for (int32_t x = 0; x < m_RenderDistance; x++)
		{
			glm::ivec2 position = { x - m_RenderDistance / 2, y - m_RenderDistance / 2 };
			m_Chunks.emplace_back(position, this);
		}
	}

	for (auto& chunk : m_Chunks)
		m_ChunksConstructDataFutures.push_back(std::async(std::launch::async, ConstructChunkData, &chunk));

	//for (auto& future : m_ChunksConstructDataFutures)
	//	future.wait();

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

Chunk* World::GetChunk(const glm::ivec2& position)
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

BlockId World::GetBlockFromPositionAbsolute(const glm::ivec3& position) const
{
	auto& spec = Chunk::GetSpecification();
	glm::ivec2 chunkPosition = { std::floor(position.x / (float)spec.Width), std::floor(position.z / (float)spec.Depth) };
	glm::ivec3 blockPosition = position - glm::ivec3(chunkPosition.x * (int32_t)spec.Width, 0, chunkPosition.y * (int32_t)spec.Depth);

	const Chunk* chunk = GetChunk(chunkPosition);
	if (chunk)
		return chunk->GetBlockAt(blockPosition);
	return BlockId::None;
}

void World::ReplaceBlockFromPositionAbsolute(const glm::ivec3& position, BlockId type)
{
	auto& spec = Chunk::GetSpecification();
	glm::ivec2 chunkPosition = { std::floor(position.x / (float)spec.Width), std::floor(position.z / (float)spec.Depth) };
	glm::ivec3 blockPosition = position - glm::ivec3(chunkPosition.x * spec.Width, 0, chunkPosition.y * spec.Depth);

	Chunk* chunk = GetChunk(chunkPosition);
	if (chunk)
	{
		BlockId oldBlock = chunk->GetBlockAt(blockPosition);
		if (oldBlock != type)
		{
			if (oldBlock == BlockId::Air)
			{
				auto& blockProperties = ChunkRenderer::GetBlockProperties().at(type);
				Entropy::AudioEngine::PlaySound(blockProperties.BreakSound);
			}
			else
			{
				auto& blockProperties = ChunkRenderer::GetBlockProperties().at(oldBlock);
				Entropy::AudioEngine::PlaySound(blockProperties.BreakSound);
			}

			chunk->ReplaceBlock(blockPosition, type);
		}
	}
}

std::tuple<BlockId, glm::ivec3, glm::ivec3> World::RayCast(const glm::vec3& start, const glm::vec3& direction, float length)
{
	glm::vec3 normDir = glm::normalize(direction);

	for (float i = 0; i < length; i += 0.01f)
	{
		glm::ivec3 position = glm::floor(start + normDir * i);
		BlockId block = GetBlockFromPositionAbsolute(position);
		if (block != BlockId::None)
		{
			if (block != BlockId::Air)
				return { block, position, glm::round(-direction) };
		}
	}
	return {};
}

void World::OnUpdate(float elapsedTime)
{
	m_Scene.OnUpdate(elapsedTime);

	for (auto& chunk : m_Chunks)
		chunk.PushData();
}

World::World()
{
	ChunkRenderer::Initialize();

	InitializeChunksAsync();
}

World::~World()
{
	ChunkRenderer::Shutdown();

	s_WorldGenerating = false;
}
