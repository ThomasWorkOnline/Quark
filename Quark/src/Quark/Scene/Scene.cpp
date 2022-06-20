#include "qkpch.h"
#include "Scene.h"
#include "Components.h"

namespace Quark {

	Scene::~Scene()
	{
		// Destroy native scripts
		{
			auto view = m_Registry.view<NativeScriptComponent>();
			for (auto entity : view)
			{
				auto& nsComponent = view.get<NativeScriptComponent>(entity);
				nsComponent.ScriptInstance->OnDestroy();
			}
		}
	}

	void Scene::OnUpdate(Timestep elapsedTime)
	{
		// Native scripts
		{
			auto view = m_Registry.view<NativeScriptComponent>();
			for (auto entity : view)
			{
				auto& nsComponent = view.get<NativeScriptComponent>(entity);
				if (!nsComponent.ScriptInstance)
				{
					nsComponent.ScriptInstance = Scope<NativeScriptEntity>(nsComponent.InstanciateScript());
					nsComponent.ScriptInstance->m_Entity = Entity{ entity, &m_Registry };
					nsComponent.ScriptInstance->OnCreate();
				}

				nsComponent.ScriptInstance->OnUpdate(elapsedTime);
			}
		}

		// Physics
		{
			auto group = m_Registry.group<Transform3DComponent, PhysicsComponent>();
			for (auto entity : group)
			{
				auto [transformComponent, physicsComponent] = group.get<Transform3DComponent, PhysicsComponent>(entity);
				physicsComponent.Velocity -= physicsComponent.Velocity * physicsComponent.Friction * (Float)elapsedTime;
				transformComponent.Position += physicsComponent.Velocity * (Float)elapsedTime;
			}
		}
	}

	void Scene::OnEvent(Event& e)
	{
		// Events on native scripts
		{
			auto view = m_Registry.view<NativeScriptComponent>();
			for (auto entity : view)
			{
				auto& nsComponent = view.get<NativeScriptComponent>(entity);
				nsComponent.ScriptInstance->OnEvent(e);
			}
		}
	}

	Entity Scene::CreateEntity()
	{
		return { m_Registry.create(), &m_Registry };
	}

	void Scene::DeleteEntity(Entity entity)
	{
		m_Registry.destroy(entity);
	}

	void Scene::SetPrimaryCamera(Entity cameraEntity)
	{
		QK_CORE_ASSERT(cameraEntity.HasComponent<CameraComponent>(), "Entity must have a camera component");
		m_CameraEntity = cameraEntity;
	}

	Ref<Scene> Scene::Create()
	{
		return CreateRef<Scene>();
	}
}
