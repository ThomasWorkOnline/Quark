#include "World.h"

#include <future>
#include <chrono>

#include "ChunkLoader.h"
#include "../Game/Resources.h"
#include "../Rendering/GameRenderer.h"

namespace VoxelCraft {

	static uint32_t s_ChunksExpected = 0;

	World::World()
	{
		QK_TIME_SCOPE_DEBUG(World::World);

		m_Loader = ChunkLoader::Create(*this, ((Position3D)m_Player.GetSettings().SpawnPoint).ToChunkCoord(), m_Player.GetSettings().RenderDistance);
	}

	World::~World()
	{

	}

	void World::OnUpdate(float elapsedTime)
	{
		const Position3D& playerPos = m_Player.GetPosition();
		m_Loader->SetCoord(playerPos.ToChunkCoord());

		// Physics
		m_Scene.OnUpdate(elapsedTime);

		ProcessPlayerCollision();

		// Controls
		m_Controller.OnUpdate(elapsedTime);

		// Environment
		m_Map.OnUpdate(elapsedTime);
		m_Loader->OnUpdate(elapsedTime);

		Quark::Renderer::BeginScene(m_Player.GetCamera().Camera.GetMatrix(),
			m_Player.GetCameraTransform());

		m_Map.Foreach([](const Chunk* data)
			{
				GameRenderer::SubmitChunk(data);
			});

		Quark::Renderer::EndScene();

		const auto& window = Quark::Application::Get().GetWindow();
		GameRenderer::DrawUI(window.GetWidth(), window.GetHeight());
	}

	void World::OnEvent(Quark::Event& e)
	{
		Quark::EventDispatcher dispatcher(e);

		dispatcher.Dispatch<Quark::KeyPressedEvent>(ATTACH_EVENT_FN(World::OnKeyPressed));
		dispatcher.Dispatch<Quark::MouseButtonPressedEvent>(ATTACH_EVENT_FN(World::OnMouseButtonPressed));

		m_Controller.OnEvent(e);
	}

	void World::OnChunkLoaded(size_t id)
	{
		// Player feels gravity when chunk is loaded
		if (((Position3D&)m_Player.GetPosition()).ToChunkCoord() == m_Map.Select(id)->GetCoord())
		{
			m_Controller.SetGravityEnabled(true);
		}
	}

	void World::OnChunkModified(size_t id)
	{
		m_Loader->Rebuild(id); // TODO: move, not related to this loader
	}

	bool World::IsPlayerTouchingGround(const Player& player) const
	{
		static constexpr float detectionTreshold = 0.01f;
		const glm::ivec3 blockUnderFeetPos = glm::floor(player.GetPosition() - glm::vec3(0.0f, detectionTreshold, 0.0f));
		auto& props = GetBlock(blockUnderFeetPos).GetProperties();

		return props.CollisionEnabled;
	}

	Quark::Scope<World> World::Create()
	{
		return Quark::CreateScope<World>();
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
			GameRenderer::SwitchShader();
			break;
		}

		return false;
	}

	bool World::OnMouseButtonPressed(Quark::MouseButtonPressedEvent& e)
	{
		Ray ray;
		ray.Origin = m_Player.GetHeadPosition();
		ray.Direction = glm::normalize(m_Player.GetTransform().GetFrontVector());

		auto data = RayCast(ray.Origin, ray.Direction, 5.0f);
		if (data.has_value())
		{
			auto& collision = data.value();

			switch (e.GetMouseButton())
			{
			case Quark::MouseCode::ButtonLeft:
				if (collision.Block != Block::ID::Air)
				{
					ReplaceBlock(collision.Impact, Block::ID::Air);
				}
				break;
			case Quark::MouseCode::ButtonRight:
				if (collision.Block != Block::ID::Air)
				{
					auto pos = collision.Impact + (glm::vec3)GetFaceNormal(collision.Side);
					if (GetBlock(pos) == Block::ID::Air)
					{
						ReplaceBlock(pos, Block::ID::Cobblestone);
					}
				}
				break;
			}
		}

		return false;
	}

	// TODO: implement a proper raycast
	std::optional<CollisionData> World::RayCast(const glm::vec3& start, const glm::vec3& direction, float length) const
	{
		glm::vec3 normDir = glm::normalize(direction);

		for (float i = 0; i < length; i += 0.01f)
		{
			glm::ivec3 position = glm::floor(start + normDir * i);
			Block block = GetBlock(position);
			if (block != Block::ID::Air)
			{
				CollisionData data;
				data.Block = block;
				data.Impact = position;
				data.Side = BlockFace::Top;

				return data;
			}
		}
		return {};
	}

	Block World::GetBlock(const Position3D& position) const
	{
		ChunkCoord coord = position.ToChunkCoord();
		Position3D blockPosition = position.ToChunkSpace(coord);

		const Chunk* chunk = m_Map.Select(CHUNK_UUID(coord));
		if (chunk && chunk->GetLoadStatus() >= Chunk::LoadStatus::WorldGenerated)
			return chunk->GetBlock(blockPosition);
		return Block::ID::Air;
	}

	void World::ReplaceBlock(const Position3D& position, Block type)
	{
		ChunkCoord coord = position.ToChunkCoord();
		Position3D blockPosition = position.ToChunkSpace(coord);

		Chunk* chunk = m_Map.Select(CHUNK_UUID(coord));
		if (chunk && chunk->GetLoadStatus() == Chunk::LoadStatus::Loaded)
		{
			Block oldBlock = chunk->GetBlock(blockPosition);
			if (oldBlock != type)
			{
				if (oldBlock == Block::ID::Air)
				{
					auto& blockProperties = type.GetProperties();
					Quark::AudioEngine::PlaySound(blockProperties.BreakSound);
				}
				else
				{
					auto& blockProperties = oldBlock.GetProperties();
					Quark::AudioEngine::PlaySound(blockProperties.BreakSound);
				}

				chunk->ReplaceBlock(blockPosition, type);
			}
		}
	}

	void World::ProcessPlayerCollision()
	{
		static constexpr float detectionTreshold = 0.01f;
		const Position3D& playerPos = m_Player.GetPosition();
		const Position3D blockUnderFeetPos = glm::floor(playerPos - Position3D(0.0f, detectionTreshold, 0.0f));

		const auto& props = GetBlock(blockUnderFeetPos).GetProperties();

		// Collide with block underneath
		if (props.CollisionEnabled)
		{
			const auto& meshProps = Resources::GetMeshProperties(props.Mesh);
			const auto& blockBounds = meshProps.Hitbox.MoveTo(blockUnderFeetPos).GetBounds();

			auto data = m_Player.GetHitbox().CollideWith(blockBounds);
			//auto data = m_Player.GetHitbox().CollideWith(glm::vec3(playerPos.x, blockBounds.Y.Max, playerPos.z));

			if (data.has_value())
			{
				auto& collision = data.value();

				m_Player.GetTransform().Position.y = blockBounds.Y.Max;
				m_Player.GetPhysics().Velocity.y = 0.f;
			}
		}
	}
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
