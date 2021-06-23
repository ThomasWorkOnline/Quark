#include "Player.h"

#include "World.h"

Player::Player(Quark::Scene& scene, const PlayerSettings& settings)
	: m_Scene(scene), m_Settings(settings)
{
	Initialize();
}

Player::~Player()
{
	m_Scene.DeleteEntity(m_Entity);
}

Quark::Transform3DComponent Player::GetCameraTransform() const
{
	auto transform = Quark::Transform3DComponent(GetTransform());
	transform.Position += m_Settings.HeadRelativeToFeet;
	return transform;
}

bool Player::IsTouchingGround() const
{
	return World::Get().IsPlayerTouchingGround(*this);
}

void Player::Initialize()
{
	auto& window = Quark::Application::Get().GetWindow();

	m_Entity = m_Scene.CreateEntity();
	m_Entity.AddComponent<Quark::Transform3DComponent>().Position = m_Settings.SpawnPoint;
	m_Entity.AddComponent<Quark::PhysicsComponent>();
	m_Entity.AddComponent<Quark::PerspectiveCameraComponent>((float)window.GetWidth() / (float)window.GetHeight(), m_Settings.Fov);
}
