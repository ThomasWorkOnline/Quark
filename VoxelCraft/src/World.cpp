#include "World.h"

#include "ChunkLoader.h"

#include <future>
#include <chrono>

static constexpr uint32_t size = 50;
static constexpr uint32_t expected = (size + 2) * (size + 2) - 4;

World::World()
{
	QK_TIME_SCOPE_DEBUG(World::World);

	ChunkRenderer::Initialize();

	m_Loader = ChunkLoader::Create(this);

	enum Orientation : int8_t
	{
		Right,
		Top,
		Left,
		Bottom
	};

	int incrementX = 1;
	int incrementZ = 0;

	float distance = 1.0f;
	int x = 0;
	int z = 0;
	int orien = 0;

	for (int i = 0; i < size; i++)
	{
		// go in straight line for the required distance
		for (int d = 0; d < (int)distance; d++)
		{
			m_Loader->Queue({ x, z });

			x += incrementX;
			z += incrementZ;
		}

		orien++;

		// turn left 90 degrees
		switch (orien % 4)
		{
		case Right:
			incrementX = 1;
			incrementZ = 0;
			distance += 0.5f;
			break;
		case Top:
			incrementX = 0;
			incrementZ = 1;
			distance += 0.5f;
			break;
		case Left:
			incrementX = -1;
			incrementZ = 0;
			distance += 0.5f;
			break;
		case Bottom:
			incrementX = 0;
			incrementZ = -1;
			distance += 0.5f;
			break;
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

	Quark::Renderer::BeginScene(GetPlayer().GetCamera().Camera.GetMatrix(),
		GetPlayer().GetTransform());

	int32_t pushCount = 0;

	static auto& chunks = m_Loader->GetLoadedChunks();
	for (auto& chunk : chunks)
	{
		if (chunk.second && chunk.second->MeshCreated())
		{
			pushCount += chunk.second->PushData();

			ChunkRenderer::SubmitChunk(chunk.second);
		}
	}

	Quark::Renderer::EndScene();
}

void World::OnEvent(Quark::Event& e)
{
	Quark::EventDispatcher dispatcher(e);

	dispatcher.Dispatch<Quark::KeyPressedEvent>(ATTACH_EVENT_FN(World::OnKeyPressed));
}

bool World::OnKeyPressed(Quark::KeyPressedEvent& e)
{
	switch (e.GetKeyCode())
	{
	case Quark::KeyCode::T:
		std::cout << m_Loader->GetStats().ChunksTerrainGenerated << " chunks terrain generated!\n";
		std::cout << m_Loader->GetStats().ChunksMeshGenerated << " chunks meshes generated!\n";
		std::cout << "chunks terrain expected: " << expected << '\n';
		std::cout << "Idling: " << m_Loader->Idling() << '\n';
		break;
	}

	return false;
}



Chunk* World::GetChunk(const glm::ivec2& position) const
{
	return m_Loader->GetDataGeneratedChunk(position);
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
CollisionData World::RayCast(const glm::vec3& start, const glm::vec3& direction, float length)
{
	CollisionData data;
	data.Block = BlockId::None;

	glm::vec3 normDir = glm::normalize(direction);

	for (float i = 0; i < length; i += 0.01f)
	{
		glm::ivec3 position = glm::floor(start + normDir * i);
		BlockId block = GetBlock(position);
		if (block != BlockId::None && block != BlockId::Air)
		{
			data.Block = block;
			data.Impact = position;
			data.Side = BlockFace::Top;

			return data;
		}
	}
	return {};
}
