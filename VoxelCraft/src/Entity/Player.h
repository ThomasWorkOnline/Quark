#pragma once

#include <Quark.h>

#include "../Utils/Position.h"

#include "../Game/Hitbox.h"

namespace VoxelCraft {

	struct PlayerSettings
	{
		float Fov = 70;
		glm::dvec3 HeadRelativeToFeet = { 0.0f, 1.75f, 0.0f };
		Position3D SpawnPoint = { 0.5, 80.0, 0.5 };
		uint32_t RenderDistance = 8;
		float FlyingMovementSpeed = 50.0f;
		float BoostFlyingMovementSpeed = 120.0f;
		float BaseMovementSpeed = 4.f;
		float SprintMovementSpeed = 800.f;
		float MouseSensitivity = 0.002f;
	};

	class World;

	class Player : public Quark::Entity
	{
	public:
		Player(World& world, Quark::Scene& scene, const PlayerSettings& settings = {});
		~Player();

		const PlayerSettings& GetSettings() const { return m_Settings; }
		Position3D GetPosition() const { return GetComponent<Quark::Transform3DComponent>().Position; }
		Position3D GetHeadPosition() { return GetPosition() + m_Settings.HeadRelativeToFeet; }

		Quark::Transform3DComponent GetCameraTransformNoPosition() const;

		HitBox GetHitbox() const;

		bool IsTouchingGround() const;

	private:
		void Initialize();

		PlayerSettings m_Settings;

		World& m_World;

		Quark::Scene& m_SceneHandle;
	};
}
