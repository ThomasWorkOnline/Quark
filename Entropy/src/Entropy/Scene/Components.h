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
			: Position(0.0f), Scale(1.0f), Orientation(glm::angleAxis(0.0f, glm::vec3(1.0f, 0.0f, 0.0f))) { }

		TransformComponent(glm::vec3 position, glm::vec3 scale, glm::quat orientation)
			: Position(position), Scale(scale), Orientation(orientation) { }

		void Rotate(const glm::quat& quat) { Orientation = Orientation * quat; }
		void Rotate(float angle, const glm::vec3 axis) { Rotate(glm::angleAxis(angle, axis)); }

		glm::mat4 GetTransform()
		{
			glm::mat4 rotation = glm::toMat4(Orientation);

			return glm::translate(glm::mat4(1.0f), Position)
				* rotation
				* glm::scale(glm::mat4(1.0f), Scale);
		}
	};

	struct PhysicsComponent
	{
		glm::vec3 Velocity;
		float Friction;

		PhysicsComponent()
			: Velocity(0.0f), Friction(4.0f)
		{
		}

		PhysicsComponent(glm::vec3 initVelocity, float coeffFriction)
			: Velocity(initVelocity), Friction(coeffFriction)
		{
		}
	};

	struct MeshComponent
	{
		Mesh Mesh;

		MeshComponent() = default;

		MeshComponent(const char* filepath)
			: Mesh(filepath)
		{
		}
	};

	struct CameraComponent
	{
		Camera Camera;

		CameraComponent(float aspectRatio, float fov)
			: Camera(aspectRatio, fov)
		{
		}
	};
}
