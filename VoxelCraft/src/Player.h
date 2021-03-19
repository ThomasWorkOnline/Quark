#pragma once

#include <Entropy.h>

struct PlayerSettings
{
	float Fov = 70;
	glm::vec3 HeadRelativeToFeet = { 0.0f, 1.0f, 0.0f };
	glm::vec3 SpawnPoint = { 0.0f, 64.0f, 0.0f };
};

class Player
{
public:
	Player(Entropy::Scene* scene, const PlayerSettings& settings = {});
	~Player();

	glm::vec3 GetHeadPosition() { return GetTransform().Position; }
	glm::vec3 GetFeetPosition() { return GetTransform().Position - m_Settings.HeadRelativeToFeet; }

	Entropy::Transform3DComponent& GetTransform() { return m_Entity.GetComponent<Entropy::Transform3DComponent>(); }
	Entropy::PhysicsComponent& GetPhysics() { return m_Entity.GetComponent<Entropy::PhysicsComponent>(); }
	Entropy::PerspectiveCameraComponent& GetCamera() { return m_Entity.GetComponent<Entropy::PerspectiveCameraComponent>(); }

	operator Entropy::Entity&() { return m_Entity; }

private:
	void Initialize();

	Entropy::Scene* m_SceneHandle;
	Entropy::Entity m_Entity;
	PlayerSettings m_Settings;
};
