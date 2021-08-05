#include "World.h"

#include <future>
#include <chrono>

#include "ChunkLoader.h"
#include "../Game/Resources.h"
#include "../Rendering/Renderer.h"

#include "../Components/GravityComponent.h"

namespace VoxelCraft {

	static uint32_t s_ChunksExpected = 0;
	static std::atomic_bool s_FlagPlayerGravity;

	World::World()
	{
		QK_TIME_SCOPE_DEBUG(World::World);

		m_Loader = ChunkLoader::Create(*this, m_Player.GetSettings().SpawnPoint.ToChunkCoord(), m_Player.GetSettings().RenderDistance);
	}

	World::~World()
	{

	}

	void World::OnUpdate(float elapsedTime)
	{
		const Position3D& playerPos = m_Player.GetPosition();
		m_Loader->SetCoord(playerPos.ToChunkCoord());

		m_Scene.OnUpdate(elapsedTime);

		ProcessPlayerCollision();

		m_Player.GetComponent<GravityComponent>().Affected = s_FlagPlayerGravity;

		// Controls
		m_Controller.OnUpdate(elapsedTime);

		// Environment
		m_Map.OnUpdate(elapsedTime);
		m_Loader->OnUpdate(elapsedTime);

		// Rendering
		Renderer::RenderMap(m_Map, m_Player.GetComponent<Quark::PerspectiveCameraComponent>().Camera.GetProjection(), m_Player.GetCameraTransformNoPosition(), m_Player.GetHeadPosition());

		const auto& window = Quark::Application::Get().GetWindow();
		Renderer::RenderUI(window.GetWidth(), window.GetHeight());
	}

	void World::OnEvent(Quark::Event& e)
	{
		Quark::EventDispatcher dispatcher(e);

		dispatcher.Dispatch<Quark::KeyPressedEvent>(ATTACH_EVENT_FN(World::OnKeyPressed));
		dispatcher.Dispatch<Quark::MouseButtonPressedEvent>(ATTACH_EVENT_FN(World::OnMouseButtonPressed));

		m_Controller.OnEvent(e);
	}

	void World::OnChunkLoaded(ChunkIdentifier id)
	{
		// Player feels gravity when chunk is loaded
		if (m_Player.GetPosition().ToChunkCoord() == m_Map.Select(id)->GetCoord())
		{
			s_FlagPlayerGravity = true;
		}
	}

	void World::OnChunkModified(ChunkIdentifier id)
	{
		m_Loader->Rebuild(id); // TODO: move, not related to this loader
	}

	bool World::IsPlayerTouchingGround(const Player& player) const
	{
		static constexpr float detectionTreshold = 0.01f;
		IntPosition3D blockUnderFeetPos = glm::floor(player.GetPosition() - Position3D(0.0f, detectionTreshold, 0.0f));
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
			std::cout << "====== WORLD SUMMARY ======\n";
			std::cout << m_Map.Count() << " chunks active\n";
			std::cout << m_Loader->GetStats().ChunksWorldGen << " chunks terrain generated\n";
			std::cout << m_Loader->GetStats().ChunksMeshGen << " chunks meshes generated\n";
			std::cout << "chunks terrain expected: " << s_ChunksExpected << '\n';
			std::cout << "Idling: " << (m_Loader->Idling() ? "true" : "false") << '\n';
			std::cout << ChunkSpecification::BlockCount * m_Loader->GetStats().ChunksWorldGen << " blocks generated\n";
			std::cout << "===========================\n";
			break;
		}

		return false;
	}

	bool World::OnMouseButtonPressed(Quark::MouseButtonPressedEvent& e)
	{
		Ray ray;
		ray.Origin = m_Player.GetHeadPosition();
		ray.Direction = glm::normalize(m_Player.GetComponent<Quark::Transform3DComponent>().GetFrontVector());

		auto data = RayCast(ray.Origin, ray.Direction, 5.0f);
		if (data.has_value())
		{
			auto& collision = data.value();

			switch (e.GetMouseButton())
			{
			case Quark::MouseCode::ButtonLeft:
				if (collision.Block != BlockID::Air)
				{
					ReplaceBlock(collision.Impact, BlockID::Air);
				}
				break;
			case Quark::MouseCode::ButtonRight:
				if (collision.Block != BlockID::Air)
				{
					auto pos = collision.Impact + (Position3D)GetFaceNormal(collision.Side);
					if (GetBlock(pos) == BlockID::Air)
					{
						ReplaceBlock(pos, BlockID::Cobblestone);
					}
				}
				break;
			}
		}

		return false;
	}

	// TODO: implement a proper raycast
	std::optional<CollisionData> World::RayCast(const Position3D& start, const glm::vec3& direction, float length) const
	{
		glm::dvec3 normDir = glm::normalize(direction);

		for (double i = 0; i < length; i += 0.01)
		{
			glm::ivec3 position = glm::floor(start + normDir * i);
			Block block = GetBlock(position);
			if (block != BlockID::Air)
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

	Block World::GetBlock(const IntPosition3D& position) const
	{
		ChunkCoord coord = position.ToChunkCoord();
		IntPosition3D blockPosition = position.ToChunkSpace(coord);

		const Chunk* chunk = m_Map.Select(ChunkIdentifier(coord));
		if (chunk && chunk->GetLoadStatus() >= Chunk::LoadStatus::WorldGenerated)
			return chunk->GetBlock(blockPosition);
		return BlockID::Air;
	}

	void World::ReplaceBlock(const IntPosition3D& position, Block type)
	{
		ChunkCoord coord = position.ToChunkCoord();
		IntPosition3D blockPosition = position.ToChunkSpace(coord);

		Chunk* chunk = m_Map.Select(ChunkIdentifier(coord));
		if (chunk && chunk->GetLoadStatus() == Chunk::LoadStatus::Loaded)
		{
			Block oldBlock = chunk->GetBlock(blockPosition);
			if (oldBlock != type)
			{
				if (chunk->ReplaceBlock(blockPosition, type))
				{
					if (oldBlock == BlockID::Air)
					{
						auto& blockProperties = type.GetProperties();
						Quark::AudioEngine::PlaySound(blockProperties.BreakSound);
					}
					else
					{
						auto& blockProperties = oldBlock.GetProperties();
						Quark::AudioEngine::PlaySound(blockProperties.BreakSound);
					}
				}
			}
		}
	}

	void World::ProcessPlayerCollision()
	{
		static constexpr float detectionTreshold = 0.01f;
		const Position3D& playerPos = m_Player.GetPosition();
		const IntPosition3D blockUnderFeetPos = glm::floor(playerPos - Position3D(0.0f, detectionTreshold, 0.0f));

		const auto& props = GetBlock(blockUnderFeetPos).GetProperties();

		// Collide with block underneath
		if (props.CollisionEnabled)
		{
			const auto& blockBounds = props.Hitbox.MoveTo(blockUnderFeetPos).GetBounds();

			auto data = m_Player.GetHitbox().CollideWith(blockBounds);
			//auto data = m_Player.GetHitbox().CollideWith(glm::vec3(playerPos.x, blockBounds.Y.Max, playerPos.z));

			if (data.has_value())
			{
				auto& collision = data.value();

				m_Player.GetComponent<Quark::Transform3DComponent>().Position.y = blockBounds.Y.Max;
				m_Player.GetComponent<Quark::PhysicsComponent>().Velocity.y = 0.f;
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
