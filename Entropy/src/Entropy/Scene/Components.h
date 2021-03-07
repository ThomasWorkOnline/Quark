#pragma once

#include "../Core/Core.h"

#include "../Renderer/Mesh.h"
#include "../Renderer/PerspectiveCamera.h"
#include "../Renderer/OrthographicCamera.h"

namespace Entropy {

	struct Transform3DComponent
	{
		glm::vec3 Position;
		glm::vec3 Scale;
		glm::quat Orientation;

		Transform3DComponent()
			: Position(0.0f), Scale(1.0f), Orientation(glm::angleAxis(0.0f, glm::vec3(0.0f, 0.0f, 1.0f))), m_Transform(1.0f) { }

		Transform3DComponent(const glm::vec3& position, const glm::vec3& scale, const glm::quat& orientation)
			: Position(position), Scale(scale), Orientation(orientation), m_Transform(1.0f) { }

		operator glm::mat4& ()
		{
			m_Transform = glm::translate(glm::mat4(1.0f), Position)
				* glm::toMat4(Orientation)
				* glm::scale(glm::mat4(1.0f), Scale);
			return m_Transform;
		}

		glm::vec3 GetFrontVector() const { return glm::vec3(0.0, 0.0f, 1.0f) * Orientation; }
		glm::vec3 GetRightVector() const { return glm::vec3(1.0, 0.0f, 0.0f) * Orientation; }
		glm::vec3 GetTopVector() const { return glm::vec3(0.0, 1.0f, 0.0f) * Orientation; }

		Transform3DComponent& Rotate(const glm::quat& quat) { Orientation *= quat; return *this; }
		Transform3DComponent& Rotate(float angle, const glm::vec3& axis) { Rotate(glm::angleAxis(glm::radians(angle), glm::normalize(axis))); return *this; }

	private:
		glm::mat4 m_Transform;
	};

	struct PhysicsComponent
	{
		glm::vec3 Velocity;
		float Friction;

		PhysicsComponent()
			: Velocity(0.0f), Friction(4.0f) { }

		PhysicsComponent(glm::vec3 initVelocity, float coeffFriction)
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
