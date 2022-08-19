#include "qkpch.h"
#include "Scene.h"

#include "Components.h"
#include "NativeScriptEntity.h"

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
				nsComponent.ScriptInstance->OnUpdate(elapsedTime);
			}
		}

		// Physics
		{
			auto group = m_Registry.group<Transform3DComponent, PhysicsComponent>();
			for (auto entity : group)
			{
				auto [transformComponent, physicsComponent] = group.get<Transform3DComponent, PhysicsComponent>(entity);
				physicsComponent.Velocity -= physicsComponent.Velocity * m_Settings.GlobalFrictionCoeff * (Float)elapsedTime;
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
		return { m_Registry.create(), this };
	}

	Entity Scene::CreatePrimaryCamera()
	{
		auto entity = CreateEntity();
		entity.AddComponent<Transform3DComponent>();
		entity.AddComponent<PhysicsComponent>();
		entity.AddComponent<CameraComponent>().Camera.SetPerspective(90.0f);
		return m_PrimaryCameraEntity = entity;
	}

	void Scene::DeleteEntity(Entity entity)
	{
		m_Registry.destroy(entity);
	}

	void Scene::SetPrimaryCamera(Entity cameraEntity)
	{
		QK_CORE_ASSERT(cameraEntity.HasComponent<CameraComponent>(), "Entity must have a camera component");
		m_PrimaryCameraEntity = cameraEntity;
	}

	template<typename Component>
	void Scene::OnComponentAdded(Entity entity, Component& c)
	{
	}

	template<typename Component>
	void Scene::OnComponentRemove(Entity entity, Component& c)
	{
	}

	template<>
	void Scene::OnComponentAdded<SpriteRendererComponent>(Entity entity, SpriteRendererComponent& src)
	{
	}

	template<>
	void Scene::OnComponentRemove<SpriteRendererComponent>(Entity entity, SpriteRendererComponent& src)
	{
	}

	template<>
	void Scene::OnComponentAdded<TexturedSpriteRendererComponent>(Entity entity, TexturedSpriteRendererComponent& tsrc)
	{
	}

	template<>
	void Scene::OnComponentRemove<TexturedSpriteRendererComponent>(Entity entity, TexturedSpriteRendererComponent& tsrc)
	{
	}

	template<>
	void Scene::OnComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent& nsc)
	{
		nsc.ScriptInstance = Scope<NativeScriptEntity>{ nsc.InstanciateScript(entity) };
		nsc.ScriptInstance->OnCreate();
	}

	template<>
	void Scene::OnComponentRemove<NativeScriptComponent>(Entity entity, NativeScriptComponent& nsc)
	{
		nsc.ScriptInstance->OnDestroy();
	}
}
