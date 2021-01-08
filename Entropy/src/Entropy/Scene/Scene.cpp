#include "Scene.h"

#include "../Core/Core.h"

#include "Entity.h"
#include "Components.h"

#include "../Renderer/Camera.h"

namespace Entropy {

	Scene::Scene()
	{
	}

	Scene::~Scene()
	{
	}

	void Scene::OnUpdate(float elapsedTime)
	{
		/*Camera* activeCamera = nullptr;
		auto cameras = m_Registry.view<TransformComponent, CameraComponent>();
		for (auto entity : cameras)
		{
			// Update Cameras
			auto [transformComponent, cameraComponent] = cameras.get<TransformComponent, CameraComponent>(entity);
			activeCamera = &cameraComponent.Camera;
		}*/

		auto entities = m_Registry.view<TransformComponent, PhysicsComponent>();
		for (auto entity : entities)
		{
			// Update physics
			auto [transformComponent, physicsComponent] = entities.get<TransformComponent, PhysicsComponent>(entity);
			physicsComponent.Velocity -= physicsComponent.Velocity * physicsComponent.Friction * elapsedTime;
			transformComponent.Position += physicsComponent.Velocity * elapsedTime;
		}
	}

	Entity Scene::CreateEntity()
	{
		return { m_Registry.create(), this };
	}

	void Scene::DeleteEntity(Entity entity)
	{
		m_Registry.destroy(entity);
	}
}
