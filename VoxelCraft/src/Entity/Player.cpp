#include "Player.h"

#include "../Game/Resources.h"
#include "../World/World.h"

#include "../Components/GravityComponent.h"

namespace VoxelCraft {

	Player::Player(World* world, Quark::Scene& scene, const PlayerSettings& settings)
		: Quark::Entity(scene.CreateEntity()), m_World(world), m_SceneHandle(scene), m_Settings(settings)
	{
		Initialize();
	}

	Player::~Player()
	{
		m_SceneHandle.DeleteEntity(*this);
	}

	Quark::Transform3DComponent Player::GetCameraTransformNoPosition() const
	{
		const auto& transform = GetComponent<Quark::Transform3DComponent>();

		return {
			{ 0.f, 0.f, 0.f },
			transform.Scale,
			transform.Orientation
		};
	}

	HitBox Player::GetHitbox() const
	{
		return Resources::GetEntityHitbox(EntityModel::Player).MoveTo(GetPosition());
	}

	bool Player::IsTouchingGround() const
	{
		return m_World->IsPlayerTouchingGround(*this);
	}

	void Player::Initialize()
	{
		auto& window = Quark::Application::Get().GetWindow();

		AddComponent<Quark::Transform3DComponent>().Position = m_Settings.SpawnPoint;
		AddComponent<Quark::PhysicsComponent>();
		AddComponent<Quark::PerspectiveCameraComponent>((float)window.GetWidth() / (float)window.GetHeight(), m_Settings.Fov);
		AddComponent<GravityComponent>(30.0f);
	}
}
