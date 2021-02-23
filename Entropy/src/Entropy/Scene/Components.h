#pragma once

#include "../Core/Core.h"

#include "../Renderer/Mesh.h"
#include "../Renderer/Camera.h"

namespace Entropy {

	struct TransformComponent
	{
		glm::vec3 Position;
		glm::vec3 Scale;
		glm::quat Orientation;

		TransformComponent()
			: Position(0.0f), Scale(1.0f), Orientation(glm::angleAxis(0.0f, glm::vec3(1.0f, 0.0f, 0.0f))), m_Transform(glm::mat4(1.0f)) { }

		TransformComponent(glm::vec3 position, glm::vec3 scale, glm::quat orientation)
			: Position(position), Scale(scale), Orientation(orientation), m_Transform(glm::mat4(1.0f)) { }

		operator glm::mat4& ()
		{
			m_Transform = glm::translate(glm::mat4(1.0f), Position)
				* glm::toMat4(Orientation)
				* glm::scale(glm::mat4(1.0f), Scale);
			return m_Transform;
		}

		TransformComponent& Rotate(const glm::quat& quat) { Orientation = Orientation * quat; return *this; }
		TransformComponent& Rotate(float angle, const glm::vec3 axis) { Rotate(glm::angleAxis(glm::radians(angle), glm::normalize(axis))); return *this; }
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

	struct CameraComponent
	{
		Camera Camera;

		CameraComponent(float aspectRatio, float fov)
			: Camera(aspectRatio, fov) { }
	};

	struct TagComponent
	{
		std::string Name;

		TagComponent(const std::string& name)
			: Name(name) { }
	};
}
