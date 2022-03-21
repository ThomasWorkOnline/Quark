#pragma once

#include "../Core/Core.h"

#include "../Renderer/Mesh.h"
#include "../Renderer/PerspectiveCamera.h"
#include "../Renderer/OrthographicCamera.h"

namespace Quark {

	struct Transform3DComponent
	{
		Vector3 Position;
		Vector3 Scale;
		Quat Orientation;

		Transform3DComponent(const Transform3DComponent& other)
			: Position(other.Position), Scale(other.Scale), Orientation(other.Orientation) {}

		Transform3DComponent()
			: Position(0.0), Scale(1.0), Orientation(glm::angleAxis<Float>(0.0f, Vector3(0.0f, 0.0f, 1.0f))) {}

		Transform3DComponent(const Vector3& position, const Vector3& scale, const Quat& orientation)
			: Position(position), Scale(scale), Orientation(orientation) {}

		// Conversion operators
		operator Mat4f() { return ComputeMatrix(); }
		operator Mat4d() { return ComputeMatrix(); }

		Vector3 GetFrontVector() const { return Vector3(0.0f, 0.0f, 1.0f) * Orientation; }
		Vector3 GetRightVector() const { return Vector3(1.0f, 0.0f, 0.0f) * Orientation; }
		Vector3 GetTopVector()   const { return Vector3(0.0f, 1.0f, 0.0f) * Orientation; }

		void SetFrontVector(const Vector3& direction)
		{
			Mat4 rotation = glm::lookAt(Position, Position + direction, { 0.f, 1.f, 0.f });
			Orientation = glm::quat_cast(rotation);
		}

		Transform3DComponent& Rotate(const Quat& quat) { Orientation = glm::normalize(Orientation * quat); return *this; }
		Transform3DComponent& Rotate(float angle, const Vector3& axis) { Rotate(glm::angleAxis<Float>(angle, glm::normalize(axis))); return *this; }

	private:
		Mat4 ComputeMatrix()
		{
			return glm::translate(Mat4(1.0f), Position)
				* glm::toMat4(Orientation)
				* glm::scale(Mat4(1.0f), Scale);
		}
	};

	struct PhysicsComponent
	{
		Vector3 Velocity;
		Float Friction;

		PhysicsComponent(const Vector3& initVelocity = Vector3(0.f), Float coeffFriction = 0.f)
			: Velocity(initVelocity), Friction(coeffFriction) {}
	};

	struct MeshComponent
	{
		Mesh MeshInstance;

		MeshComponent() = default;
		MeshComponent(std::string_view filepath)
			: MeshInstance(filepath) { }
	};

	struct PerspectiveCameraComponent
	{
		PerspectiveCamera Camera;

		PerspectiveCameraComponent(float aspectRatio, float fov)
			: Camera(aspectRatio, fov) {}
	};

	struct OrthographicCameraComponent
	{
		OrthographicCamera Camera;

		OrthographicCameraComponent(float aspectRatio, float zoom)
			: Camera(aspectRatio, zoom) {}
	};

	struct TagComponent
	{
		std::string Name;

		TagComponent(const std::string& name)
			: Name(name) {}
	};
}
