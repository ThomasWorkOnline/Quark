#include "World.h"

#include "ChunkLoader.h"

#include <future>
#include <chrono>

World::World()
{
	QK_TIME_SCOPE_DEBUG(World::World);

	ChunkRenderer::Initialize();

	m_Loader = ChunkLoader::Create(this);

	constexpr uint32_t size = 50;
	for (uint32_t z = 0; z < size; z++)
	{
		for (uint32_t x = 0; x < size; x++)
		{
			m_Loader->Queue({ x, z });
		}
	}
}

World::~World()
{
	ChunkRenderer::Shutdown();
}

void World::OnUpdate(float elapsedTime)
{
	m_Scene.OnUpdate(elapsedTime);
	glm::vec3 playerPos = m_Player.GetFeetPosition();

	auto& chunks = m_Loader->GetChunks();
	for (auto chunk : chunks)
	{
		if (chunk.second) ChunkRenderer::SubmitChunk(chunk.second);
	}
}

void World::OnEvent(Quark::Event& e)
{
	Quark::EventDispatcher dispatcher(e);

	dispatcher.Dispatch<Quark::KeyPressedEvent>(ATTACH_EVENT_FN(World::OnKeyPressed));
}

bool World::OnKeyPressed(Quark::KeyPressedEvent& e)
{
	return false;
}



Chunk* World::GetChunk(const glm::ivec2& position) const
{
	return m_Loader->GetChunk(position);
}

void World::ReplaceBlock(const glm::ivec3& position, BlockId type)
{
	auto& spec = Chunk::GetSpecification();
	glm::ivec2 chunkPosition = GetChunkCoord(position);
	glm::ivec3 blockPosition = position - glm::ivec3(chunkPosition.x * spec.Width, 0, chunkPosition.y * spec.Depth);

	Chunk* chunk = GetChunk(chunkPosition);
	if (chunk)
	{
		BlockId oldBlock = chunk->GetBlock(blockPosition);
		if (oldBlock != type)
		{
			if (oldBlock == BlockId::Air)
			{
				auto& blockProperties = ChunkRenderer::GetBlockProperties().at(type);
				Quark::AudioEngine::PlaySound(blockProperties.BreakSound);
			}
			else
			{
				auto& blockProperties = ChunkRenderer::GetBlockProperties().at(oldBlock);
				Quark::AudioEngine::PlaySound(blockProperties.BreakSound);
			}

			chunk->ReplaceBlock(blockPosition, type);
		}
	}
}

BlockId World::GetBlock(const glm::ivec3& position) const
{
	auto& spec = Chunk::GetSpecification();
	glm::ivec2 chunkPosition = GetChunkCoord(position);
	glm::ivec3 blockPosition = position - glm::ivec3(chunkPosition.x * (int32_t)spec.Width, 0, chunkPosition.y * (int32_t)spec.Depth);

	Chunk* chunk = GetChunk(chunkPosition);
	if (chunk)
		return chunk->GetBlock(blockPosition);
	return BlockId::None;
}

glm::ivec2 World::GetChunkCoord(const glm::ivec3& position) const
{
	auto& spec = Chunk::GetSpecification();
	return { std::floor(position.x / (float)spec.Width), std::floor(position.z / (float)spec.Depth) };
}

bool World::HasGeneratedChunk(const glm::ivec2& position) const
{
	auto queryChunk = GetChunk(position);
	return queryChunk && queryChunk->MeshCreated();
}



// TODO: implement a proper raycast
std::tuple<BlockId, glm::ivec3, glm::ivec3> World::RayCast(const glm::vec3& start, const glm::vec3& direction, float length)
{
	glm::vec3 normDir = glm::normalize(direction);

	for (float i = 0; i < length; i += 0.01f)
	{
		glm::ivec3 position = glm::floor(start + normDir * i);
		BlockId block = GetBlock(position);
		if (block != BlockId::None)
		{
			if (block != BlockId::Air)
				return { block, position, glm::round(-direction) };
		}
	}
	return {};
}
