#pragma once

#include "../Core/Core.h"

#include "../Renderer/Mesh.h"
#include "../Renderer/Sprite.h"
#include "../Renderer/Camera.h"

namespace Entropy {

	struct TransformComponent
	{
		glm::vec3 Position;
		glm::vec3 Scale;
		glm::quat Orientation;
	public:
		TransformComponent()
			: Position(0.0f), Scale(1.0f), Orientation(glm::angleAxis(0.0f, glm::vec3(1.0f, 0.0f, 0.0f))), m_Transform(glm::mat4(1.0f)) { }

		TransformComponent(glm::vec3 position, glm::vec3 scale, glm::quat orientation)
			: Position(position), Scale(scale), Orientation(orientation), m_Transform(glm::mat4(1.0f)) { }

		inline operator glm::mat4& ()
		{
			m_Transform = glm::translate(glm::mat4(1.0f), Position)
				* glm::toMat4(Orientation)
				* glm::scale(glm::mat4(1.0f), Scale);
			return m_Transform;
		}

		void Rotate(const glm::quat& quat) { Orientation = Orientation * quat; }
		void Rotate(float angle, const glm::vec3 axis) { Rotate(glm::angleAxis(angle, axis)); }
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

		MeshComponent(const char* filepath)
			: Mesh(filepath) { }
	};

	struct SpriteComponent
	{
		Sprite Sprite;

		SpriteComponent() = default;
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

		TagComponent(const char* name)
			: Name(name) { }
	};
}
