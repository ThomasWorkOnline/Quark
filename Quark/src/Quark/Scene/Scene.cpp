#include "qkpch.h"
#include "Scene.h"
#include "Components.h"

namespace Quark {

	void Scene::OnUpdate(Timestep elapsedTime)
	{
		auto entities = m_Registry.group<Transform3DComponent, PhysicsComponent>();
		for (auto entity : entities)
		{
			auto [transformComponent, physicsComponent] = entities.get<Transform3DComponent, PhysicsComponent>(entity);
			physicsComponent.Velocity -= physicsComponent.Velocity * physicsComponent.Friction * (Float)elapsedTime;
			transformComponent.Position += physicsComponent.Velocity * (Float)elapsedTime;
		}
	}

	Entity Scene::CreateEntity()
	{
		return { m_Registry.create(), *this };
	}

	void Scene::DeleteEntity(Entity entity)
	{
		m_Registry.destroy(entity);
	}

	void Scene::SetPrimaryCamera(Entity cameraEntity)
	{
		m_CameraEntity = cameraEntity;
	}

	Ref<Scene> Scene::Create()
	{
		return CreateRef<Scene>();
	}
}
