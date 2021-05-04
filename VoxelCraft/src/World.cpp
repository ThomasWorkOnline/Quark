#include "World.h"

#include <future>
#include <chrono>

static std::mutex s_RenderQueueMutex;
static void LoadChunks(World* world)
{
	std::lock_guard<std::mutex> lock(s_RenderQueueMutex);

	glm::ivec2 chunkCoord = world->GetChunkCoordFromPositionAbsolute(world->GetPlayer().GetFeetPosition());
	auto queryChunk = world->GetChunk(chunkCoord);
	if (!queryChunk || !queryChunk->Loaded())
	{
		Chunk* newChunk = Chunk::Load(chunkCoord, world);

		world->GetRenderableChunks().push_back(newChunk);
	}
}

static void UnloadChunks(World* world)
{
	auto& renderableChunks = world->GetRenderableChunks();
	// Test
	if (!renderableChunks.empty())
	{
		renderableChunks.erase(renderableChunks.begin());
	}
}

World::World()
{
	QK_TIME_SCOPE_DEBUG(World::World);

	ChunkRenderer::Initialize();

	//m_Chunks.reserve((size_t)m_RenderDistance * (size_t)m_RenderDistance);
}

World::~World()
{
	for (auto chunk : m_Chunks)
	{
		delete chunk;
	}

	ChunkRenderer::Shutdown();
}


static std::future<void> s_ChunkUnloadFuture;
void World::OnUpdate(float elapsedTime)
{
	m_Scene.OnUpdate(elapsedTime);
	glm::vec3 playerPos = m_Player.GetFeetPosition();

	m_WorldGenFuture = std::async(std::launch::async, LoadChunks, this);

	for (auto chunk : m_RenderableChunks)
	{
		ChunkRenderer::SubmitChunk(chunk);
	}
}

void World::OnEvent(Quark::Event& e)
{
	Quark::EventDispatcher dispatcher(e);

	dispatcher.Dispatch<Quark::KeyPressedEvent>(ATTACH_EVENT_FN(World::OnKeyPressed));
}

bool World::OnKeyPressed(Quark::KeyPressedEvent& e)
{
	if (Quark::Input::IsKeyPressed(Quark::Key::T))
	{
		UnloadChunks(this);
	}
	
	return false;
}


Chunk* World::GetChunk(const glm::ivec2& position) const
{
	for (auto chunk : m_Chunks)
	{
		if (chunk->GetPosition() == position)
		{
			return chunk;
		}
	}
	return nullptr;
}



void World::ReplaceBlockFromPositionAbsolute(const glm::ivec3& position, BlockId type)
{
	auto& spec = Chunk::GetSpecification();
	glm::ivec2 chunkPosition = GetChunkCoordFromPositionAbsolute(position);
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

BlockId World::GetBlockFromPositionAbsolute(const glm::ivec3& position) const
{
	auto& spec = Chunk::GetSpecification();
	glm::ivec2 chunkPosition = GetChunkCoordFromPositionAbsolute(position);
	glm::ivec3 blockPosition = position - glm::ivec3(chunkPosition.x * (int32_t)spec.Width, 0, chunkPosition.y * (int32_t)spec.Depth);

	Chunk* chunk = GetChunk(chunkPosition);
	if (chunk)
		return chunk->GetBlockAt(blockPosition);
	return BlockId::None;
}

glm::ivec2 World::GetChunkCoordFromPositionAbsolute(const glm::ivec3& position) const
{
	auto& spec = Chunk::GetSpecification();
	return { std::floor(position.x / (float)spec.Width), std::floor(position.z / (float)spec.Depth) };
}



// TODO: implement a proper raycast
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

void World::AddChunk(Chunk* chunk)
{
	m_Chunks.push_back(chunk);
}

void World::RemoveChunk(Chunk* chunk)
{
	
}
