#pragma once

#include "../Core/Core.h"

#include "../Renderer/Mesh.h"
#include "../Renderer/PerspectiveCamera.h"
#include "../Renderer/OrthographicCamera.h"

namespace Quark {

	struct Transform3DComponent
	{
		glm::dvec3 Position;
		glm::dvec3 Scale;
		glm::dquat Orientation;

		Transform3DComponent(const Transform3DComponent& other)
			: Position(other.Position), Scale(other.Scale), Orientation(other.Orientation), m_Transform(1.0f) {}

		Transform3DComponent()
			: Position(0.0f), Scale(1.0f), Orientation(glm::angleAxis(0.0f, glm::vec3(0.0f, 0.0f, 1.0f))), m_Transform(1.0f) { }

		Transform3DComponent(const glm::vec3& position, const glm::vec3& scale, const glm::quat& orientation)
			: Position(position), Scale(scale), Orientation(orientation), m_Transform(1.0f) { }

		operator glm::dmat4& ()
		{
			UpdateMatrix();
			return m_Transform;
		}

		operator glm::mat4& ()
		{
			UpdateMatrix();
			return (glm::mat4&)m_Transform;
		}

		glm::dvec3 GetFrontVector() const { return glm::dvec3(0.0, 0.0f, 1.0f) * Orientation; }
		glm::dvec3 GetRightVector() const { return glm::dvec3(1.0, 0.0f, 0.0f) * Orientation; }
		glm::dvec3 GetTopVector() const { return glm::dvec3(0.0, 1.0f, 0.0f) * Orientation; }

		void SetFrontVector(const glm::dvec3& direction)
		{
			glm::dmat4 rotation = glm::lookAt(Position, Position + direction, { 0, 1, 0 });
			Orientation = glm::quat_cast(rotation);
		}

		Transform3DComponent& Rotate(const glm::dquat& quat) { Orientation = glm::normalize(Orientation * quat); return *this; }
		Transform3DComponent& Rotate(double angle, const glm::dvec3& axis) { Rotate(glm::angleAxis(angle, glm::normalize(axis))); return *this; }

	private:
		void UpdateMatrix()
		{
			m_Transform = glm::translate(glm::dmat4(1.0f), Position)
				* glm::toMat4(Orientation)
				* glm::scale(glm::dmat4(1.0f), Scale);
		}

		glm::dmat4 m_Transform;
	};

	struct PhysicsComponent
	{
		glm::dvec3 Velocity;
		double Friction;

		PhysicsComponent()
			: Velocity(0.0f), Friction(0.0f) { }

		PhysicsComponent(glm::dvec3 initVelocity, double coeffFriction)
			: Velocity(initVelocity), Friction(coeffFriction) { }
	};

	struct MeshComponent
	{
		Mesh Mesh;

		MeshComponent() = default;
		MeshComponent(const BufferLayout& layout, const std::string& filepath)
			: Mesh(layout, filepath) { }
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
