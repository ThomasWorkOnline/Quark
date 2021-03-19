#include "Player.h"

Player::Player(Entropy::Scene* scene, const PlayerSettings& settings)
	: m_SceneHandle(scene), m_Settings(settings)
{
	Initialize();
}

Player::~Player()
{
}

void Player::Initialize()
{
	auto& window = Entropy::Application::Get().GetWindow();

	m_Entity = m_SceneHandle->CreateEntity();
	m_Entity.AddComponent<Entropy::Transform3DComponent>().Position = m_Settings.SpawnPoint + m_Settings.HeadRelativeToFeet;
	m_Entity.AddComponent<Entropy::PhysicsComponent>();
	m_Entity.AddComponent<Entropy::PerspectiveCameraComponent>((float)window.GetWidth() / (float)window.GetHeight(), m_Settings.Fov);
}
