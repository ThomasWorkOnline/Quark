#include "Scene.h"

#include "../Core/Core.h"

#include "Entity.h"
#include "Components.h"

#include "../Renderer/PerspectiveCamera.h"
#include "../Renderer/OrthographicCamera.h"

namespace Quark {

	Scene::Scene()
	{
	}

	Scene::~Scene()
	{
	}

	void Scene::OnUpdate(Float elapsedTime)
	{
		{
			auto entities = m_Registry.view<Transform3DComponent, PhysicsComponent>();
			for (auto entity : entities)
			{
				auto [transformComponent, physicsComponent] = entities.get<Transform3DComponent, PhysicsComponent>(entity);
				physicsComponent.Velocity -= physicsComponent.Velocity * physicsComponent.Friction * elapsedTime;
				transformComponent.Position += physicsComponent.Velocity * elapsedTime;
			}
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
}
