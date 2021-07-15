#include "Player.h"

#include "../Game/Resources.h"
#include "../World/World.h"

namespace VoxelCraft {

	Player::Player(World& world, Quark::Scene& scene, const PlayerSettings& settings)
		: m_Scene(scene), m_Settings(settings), m_World(world)
	{
		Initialize();
	}

	Player::~Player()
	{
		m_Scene.DeleteEntity(m_Entity);
	}

	Quark::Transform3DComponent Player::GetCameraTransform() const
	{
		auto transform = GetTransform(); // Copy transform
		transform.Position += m_Settings.HeadRelativeToFeet;
		return transform;
	}

	const HitBox& Player::GetHitbox() const
	{
		return Resources::GetPlayerHitbox().MoveTo(GetPosition());
	}

	bool Player::IsTouchingGround() const
	{
		return m_World.IsPlayerTouchingGround(*this);
	}

	void Player::Initialize()
	{
		auto& window = Quark::Application::Get().GetWindow();

		m_Entity = m_Scene.CreateEntity();
		m_Entity.AddComponent<Quark::Transform3DComponent>().Position = m_Settings.SpawnPoint;
		m_Entity.AddComponent<Quark::PhysicsComponent>();
		m_Entity.AddComponent<Quark::PerspectiveCameraComponent>((float)window.GetWidth() / (float)window.GetHeight(), m_Settings.Fov);
	}
}
