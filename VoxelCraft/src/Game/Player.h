#pragma once

#include <Quark.h>

#include "Hitbox.h"

struct PlayerSettings
{
	float Fov = 70;
	glm::vec3 HeadRelativeToFeet = { 0.0f, 1.75f, 0.0f };
	glm::vec3 SpawnPoint = { 0.5f, 80.0f, 0.5f };
	uint32_t RenderDistance = 12;
	float FlyingMovementSpeed = 50.0f;
	float BoostFlyingMovementSpeed = 120.0f;
	float BaseMovementSpeed = 15.0f;
	float SprintMovementSpeed = 20.0f;
	float MouseSensitivity = 0.002f;
};

class World;

class Player
{
public:
	Player(World& world, Quark::Scene& scene, const PlayerSettings& settings = {});
	~Player();

	const PlayerSettings& GetSettings() const { return m_Settings; }
	const glm::vec3& GetPosition() const { return GetTransform().Position; }
	glm::vec3 GetHeadPosition() { return GetTransform().Position + m_Settings.HeadRelativeToFeet; }

	const Quark::Transform3DComponent& GetTransform() const { return m_Entity.GetComponent<Quark::Transform3DComponent>(); }
	Quark::Transform3DComponent& GetTransform() { return m_Entity.GetComponent<Quark::Transform3DComponent>(); }
	Quark::Transform3DComponent GetCameraTransform() const;

	const Quark::PhysicsComponent& GetPhysics() const { return m_Entity.GetComponent<Quark::PhysicsComponent>(); }
	Quark::PhysicsComponent& GetPhysics() { return m_Entity.GetComponent<Quark::PhysicsComponent>(); }

	const Quark::PerspectiveCameraComponent& GetCamera() const { return m_Entity.GetComponent<Quark::PerspectiveCameraComponent>(); }
	const HitBox& GetHitbox() const;

	bool IsTouchingGround() const;

	operator Quark::Entity() const { return m_Entity; }

private:
	void Initialize();

	Quark::Scene& m_Scene;
	Quark::Entity m_Entity;
	PlayerSettings m_Settings;

	World& m_World;
};
