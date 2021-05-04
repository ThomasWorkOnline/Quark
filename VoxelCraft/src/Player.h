#pragma once

#include <Quark.h>

struct PlayerSettings
{
	float Fov = 70;
	glm::vec3 HeadRelativeToFeet = { 0.0f, 1.0f, 0.0f };
	glm::vec3 SpawnPoint = { 0.0f, 65.0f, 0.0f };
};

class Player
{
public:
	Player(Quark::Scene& scene, const PlayerSettings& settings = {});
	~Player();

	const glm::vec3& GetHeadPosition() { return GetTransform().Position; }
	glm::vec3 GetFeetPosition() { return GetTransform().Position - m_Settings.HeadRelativeToFeet; }

	Quark::Transform3DComponent& GetTransform() { return m_Entity.GetComponent<Quark::Transform3DComponent>(); }
	Quark::PhysicsComponent& GetPhysics() { return m_Entity.GetComponent<Quark::PhysicsComponent>(); }
	Quark::PerspectiveCameraComponent& GetCamera() { return m_Entity.GetComponent<Quark::PerspectiveCameraComponent>(); }

	operator Quark::Entity() { return m_Entity; }

private:
	void Initialize();

	Quark::Scene& m_Scene;
	Quark::Entity m_Entity;
	PlayerSettings m_Settings;
};
