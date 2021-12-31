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
			: Position(0.0), Scale(1.0), Orientation(glm::angleAxis(FloatingType(0.0), Vector3(0.0f, 0.0f, 1.0f))) { }

		Transform3DComponent(const Vector3& position, const Vector3& scale, const Quat& orientation)
			: Position(position), Scale(scale), Orientation(orientation) { }

		operator Mat4()
		{
			return ComputeMatrix();
		}

		Vector3 GetFrontVector() const { return Vector3(0.0, 0.0, 1.0) * Orientation; }
		Vector3 GetRightVector() const { return Vector3(1.0, 0.0, 0.0) * Orientation; }
		Vector3 GetTopVector() const   { return Vector3(0.0, 1.0, 0.0) * Orientation; }

		void SetFrontVector(const Vector3& direction)
		{
			Mat4 rotation = glm::lookAt(Position, Position + direction, { 0, 1, 0 });
			Orientation = glm::quat_cast(rotation);
		}

		Transform3DComponent& Rotate(const Quat& quat) { Orientation = glm::normalize(Orientation * quat); return *this; }
		Transform3DComponent& Rotate(FloatingType angle, const Vector3& axis) { Rotate(glm::angleAxis(angle, glm::normalize(axis))); return *this; }

	private:
		Mat4f ComputeMatrix()
		{
			return glm::translate(Mat4(1.0), Position)
				* glm::toMat4(Orientation)
				* glm::scale(Mat4(1.0), Scale);
		}
	};

	struct PhysicsComponent
	{
		Vector3 Velocity;
		FloatingType Friction;

		PhysicsComponent()
			: Velocity(0.0), Friction(0.0) { }

		PhysicsComponent(const Vector3& initVelocity, FloatingType coeffFriction)
			: Velocity(initVelocity), Friction(coeffFriction) { }
	};

	struct MeshComponent
	{
		Mesh Mesh;

		MeshComponent() = default;
		MeshComponent(const std::string& filepath)
			: Mesh(filepath) { }
	};

	struct PerspectiveCameraComponent
	{
		PerspectiveCamera Camera;

		PerspectiveCameraComponent(float aspectRatio, float fov)
			: Camera(aspectRatio, fov) { }
	};

	struct OrthographicCameraComponent
	{
		OrthographicCamera Camera;

		OrthographicCameraComponent(float aspectRatio, float zoom)
			: Camera(aspectRatio, zoom) { }
	};

	struct TagComponent
	{
		std::string Name;

		TagComponent(const std::string& name)
			: Name(name) { }
	};
}
