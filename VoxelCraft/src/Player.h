#pragma once

#include <Quark.h>

struct PlayerSettings
{
	float Fov = 70;
	glm::vec3 HeadRelativeToFeet = { 0.0f, 1.0f, 0.0f };
	glm::vec3 SpawnPoint = { 0.5f, 65.0f, 0.5f };
	float BaseMovementSpeed = 100.0f;
	float SprintMovementSpeed = 500.0f;
	float MouseSensitivity = 0.002f;
};

class Player
{
public:
	Player(Quark::Scene& scene, const PlayerSettings& settings = {});
	~Player();

	const PlayerSettings& GetSettings() const { return m_Settings; }
	const glm::vec3& GetPosition() { return GetTransform().Position; }
	glm::vec3 GetFeetPosition() { return GetTransform().Position - m_Settings.HeadRelativeToFeet; }

	Quark::Transform3DComponent& GetTransform() { return m_Entity.GetComponent<Quark::Transform3DComponent>(); }
	Quark::PhysicsComponent& GetPhysics() { return m_Entity.GetComponent<Quark::PhysicsComponent>(); }
	Quark::PerspectiveCameraComponent& GetCamera() { return m_Entity.GetComponent<Quark::PerspectiveCameraComponent>(); }

	operator Quark::Entity() const { return m_Entity; }

private:
	void Initialize();

	Quark::Scene& m_Scene;
	Quark::Entity m_Entity;
	PlayerSettings m_Settings;
};
