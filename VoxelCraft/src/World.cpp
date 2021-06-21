#include "World.h"

#include <future>
#include <chrono>

#include "ChunkLoader.h"
#include "ChunkRenderer.h"
#include "Resources.h"

static uint32_t s_ChunksExpected = 0;

World::World()
	: m_Map(this)
{
	QK_TIME_SCOPE_DEBUG(World::World);

	Resources::Initialize();
	ChunkRenderer::Initialize();

	m_Loader = ChunkLoader::Create(this, { 0, 0 }, m_Player.GetSettings().RenderDistance);
}

World::~World()
{
	ChunkRenderer::Shutdown();
}

void World::OnUpdate(float elapsedTime)
{
	glm::ivec2 coord = {
		GetChunkCoord(m_Player.GetPosition()).x,
		GetChunkCoord(m_Player.GetPosition()).y
	};
	m_Loader->SetCoord(coord);

	m_Scene.OnUpdate(elapsedTime);
	m_Map.OnUpdate(elapsedTime);
	m_Loader->OnUpdate(elapsedTime);

	Quark::Renderer::BeginScene(m_Player.GetCamera().Camera.GetMatrix(),
		m_Player.GetTransform());

	m_Map.Foreach([](const Chunk* data)
		{
			ChunkRenderer::Submit(data);
		});

	Quark::Renderer::EndScene();
}

void World::OnEvent(Quark::Event& e)
{
	Quark::EventDispatcher dispatcher(e);

	dispatcher.Dispatch<Quark::KeyPressedEvent>(ATTACH_EVENT_FN(World::OnKeyPressed));
	dispatcher.Dispatch<Quark::MouseButtonPressedEvent>(ATTACH_EVENT_FN(World::OnMouseButtonPressed));
}

void World::OnChunkModified(glm::ivec2 coord)
{
	m_Loader->Rebuild(coord); // TODO: move, not related to this loader
}

bool World::OnKeyPressed(Quark::KeyPressedEvent& e)
{
	switch (e.GetKeyCode())
	{
	case Quark::KeyCode::T:
		std::cout << m_Loader->GetStats().ChunksWorldGen << " chunks terrain generated!\n";
		std::cout << m_Loader->GetStats().ChunksMeshGen << " chunks meshes generated!\n";
		std::cout << "chunks terrain expected: " << s_ChunksExpected << '\n';
		std::cout << "Idling: " << (m_Loader->Idling() ? "true" : "false") << '\n';
		std::cout << ChunkSpecification::BlockCount * m_Loader->GetStats().ChunksWorldGen << " blocks generated!\n";
		break;
	case Quark::KeyCode::F1:
	{
		ChunkRenderer::SwitchShader();
		break;
	}
	case Quark::KeyCode::X:
	{
		auto coord = GetChunkCoord(m_Player.GetPosition());
		m_Loader->Unload(coord);
		break;
	}
	case Quark::KeyCode::Z:
	{
		auto coord = GetChunkCoord(m_Player.GetPosition());
		m_Loader->Load(coord);
		break;
	}
	case Quark::KeyCode::S:
	{
		if (Quark::Input::IsKeyPressed(Quark::KeyCode::LeftControl))
		{
			auto coord = GetChunkCoord(m_Player.GetPosition());
			Chunk* chunk = m_Map.Select(coord);
			chunk->Save();
		}
		break;
	}
	}

	return false;
}

bool World::OnMouseButtonPressed(Quark::MouseButtonPressedEvent& e)
{
	if (Quark::Input::IsKeyPressed(Quark::KeyCode::LeftControl))
	{
		switch (e.GetMouseButton())
		{
		case Quark::MouseCode::ButtonRight:
			
			break;
		case Quark::MouseCode::ButtonLeft:
			
			break;
		}
	}
	
	return false;
}

void World::ReplaceBlock(const glm::ivec3& position, Block type)
{
	glm::ivec2 chunkPosition = GetChunkCoord(position);
	glm::ivec3 blockPosition = position - glm::ivec3(chunkPosition.x * ChunkSpecification::Width, 0, chunkPosition.y * ChunkSpecification::Depth);

	Chunk* chunk = m_Map.Select(chunkPosition);
	if (chunk && chunk->GetLoadStatus() == Chunk::LoadStatus::Loaded)
	{
		Block oldBlock = chunk->GetBlock(blockPosition);
		if (oldBlock != type)
		{
			if (oldBlock == Block::Air)
			{
				auto& blockProperties = Resources::GetBlockProperties().at(type);
				Quark::AudioEngine::PlaySound(blockProperties.BreakSound);
			}
			else
			{
				auto& blockProperties = Resources::GetBlockProperties().at(oldBlock);
				Quark::AudioEngine::PlaySound(blockProperties.BreakSound);
			}

			chunk->ReplaceBlock(blockPosition, type);
		}
	}
}

Block World::GetBlock(const glm::ivec3& position) const
{
	glm::ivec2 coord = GetChunkCoord(position);
	glm::ivec3 blockPosition = position - glm::ivec3(coord.x * (int32_t)ChunkSpecification::Width, 0, coord.y * (int32_t)ChunkSpecification::Depth);

	Chunk* chunk = m_Map.Select(coord);
	if (chunk)
		return chunk->GetBlock(blockPosition);
	return Block::None;
}

glm::ivec2 World::GetChunkCoord(const glm::ivec3& position)
{
	return { std::floor(position.x / (float)ChunkSpecification::Width), std::floor(position.z / (float)ChunkSpecification::Depth) };
}

// TODO: implement a proper raycast
CollisionData World::RayCast(const glm::vec3& start, const glm::vec3& direction, float length)
{
	CollisionData data;
	data.Block = Block::None;

	glm::vec3 normDir = glm::normalize(direction);

	for (float i = 0; i < length; i += 0.01f)
	{
		glm::ivec3 position = glm::floor(start + normDir * i);
		Block block = GetBlock(position);
		if (block != Block::None && block != Block::Air)
		{
			data.Block = block;
			data.Impact = position;
			data.Side = BlockFace::Top;

			return data;
		}
	}
	return {};
}

/*void World::LoadAroundPlayer()
{
	int incrementX = 1;
	int incrementZ = 0;

	float distance = 1.0f;
	int x = 0;
	int z = 0;
	int orien = 0;

	for (int i = 0; i < m_RenderDistance; i++)
	{
		// go in straight line for the required distance
		for (int d = 0; d < (int)distance; d++)
		{
			auto playerChunkCoord = GetChunkCoord(m_Player.GetPosition());
			m_Loader->Load({ x + playerChunkCoord.x, z + playerChunkCoord.y });
			s_ChunksExpected++;

			x += incrementX;
			z += incrementZ;
		}

		orien++;

		// turn left 90 degrees
		switch (orien % 4)
		{
		case 0:
			incrementX = 1;
			incrementZ = 0;
			distance += 0.5f;
			break;
		case 1:
			incrementX = 0;
			incrementZ = 1;
			distance += 0.5f;
			break;
		case 2:
			incrementX = -1;
			incrementZ = 0;
			distance += 0.5f;
			break;
		case 3:
			incrementX = 0;
			incrementZ = -1;
			distance += 0.5f;
			break;
		}
	}
}*/
