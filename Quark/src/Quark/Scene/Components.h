#pragma once

#include "Quark/Core/Core.h"

#include "SceneCamera.h"
#include "NativeScriptEntity.h"
#include "Quark/Renderer/Mesh.h"

namespace Quark {

	struct CameraComponent
	{
		SceneCamera Camera;
	};

	struct TagComponent
	{
		std::string Name;

		TagComponent(std::string name)
			: Name(std::move(name)) {}
	};

	struct Transform3DComponent
	{
		Vec3 Position;
		Vec3 Scale;
		Quat Orientation;

		Transform3DComponent(const Transform3DComponent& other)
			: Position(other.Position), Scale(other.Scale), Orientation(other.Orientation) {}

		Transform3DComponent()
			: Position(0.0), Scale(1.0), Orientation(glm::angleAxis<Float>(0.0f, Vec3(0.0f, 0.0f, 1.0f))) {}

		Transform3DComponent(const Vec3& position, const Vec3& scale, const Quat& orientation)
			: Position(position), Scale(scale), Orientation(orientation) {}

		// Conversion operators
		operator Mat4() const { return GetMatrix(); }
		Mat4 GetMatrix() const { return ComputeMatrix(); }

		Vec3 GetFrontVector() const { return Vec3(0.0f, 0.0f, 1.0f) * Orientation; }
		Vec3 GetRightVector() const { return Vec3(1.0f, 0.0f, 0.0f) * Orientation; }
		Vec3 GetTopVector()   const { return Vec3(0.0f, 1.0f, 0.0f) * Orientation; }

		void SetFrontVector(const Vec3& direction)
		{
			Mat4 rotation = glm::lookAt(Position, Position + direction, { 0.f, 1.f, 0.f });
			Orientation = glm::quat_cast(rotation);
		}

		Transform3DComponent& Rotate(const Quat& quat) { Orientation = glm::normalize(Orientation * quat); return *this; }
		Transform3DComponent& Rotate(float angle, const Vec3& axis) { Rotate(glm::angleAxis<Float>(angle, glm::normalize(axis))); return *this; }

	private:
		Mat4 ComputeMatrix() const
		{
			return glm::translate(Mat4(1.0f), Position)
				* glm::toMat4(Orientation)
				* glm::scale(Mat4(1.0f), Scale);
		}
	};

	struct PhysicsComponent
	{
		Vec3  Velocity;
		Float Friction;

		PhysicsComponent(const Vec3& initVelocity = Vec3(0.f), Float coeffFriction = 0.f)
			: Velocity(initVelocity), Friction(coeffFriction) {}
	};

	struct MeshComponent
	{
		Mesh MeshInstance;

		MeshComponent() = default;
	};

	struct NativeScriptComponent
	{
		Scope<NativeScriptEntity> ScriptInstance;
		NativeScriptEntity* (*InstanciateScript)();

		template<typename T>
		void Bind()
		{
			static_assert(std::is_base_of_v<NativeScriptEntity, T>,
				"Template argument must be a subtype of NativeScriptEntity");

			InstanciateScript = []() { return (NativeScriptEntity*)(new T()); };
		}
	};
}
