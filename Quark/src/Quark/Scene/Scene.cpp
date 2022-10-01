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

	void Scene::OnUpdate(Timestep elapsedTime)
	{
		UpdateNativeScripts(elapsedTime);
		UpdatePhysicsBodies(elapsedTime);
		UpdateTransforms(elapsedTime);
	}

	Entity Scene::CreateEntity()
	{
		return { m_Registry.create(), this };
	}

	void Scene::DeleteEntity(Entity entity)
	{
		m_Registry.destroy(entity);
	}

	void Scene::UpdateNativeScripts(Timestep elapsedTime)
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
	}

	void Scene::UpdatePhysicsBodies(Timestep elapsedTime)
	{
		// Physics
		{
			auto group = m_Registry.view<PhysicsComponent>();
			for (auto entity : group)
			{
				auto& physicsComponent = group.get<PhysicsComponent>(entity);
				physicsComponent.Velocity -= physicsComponent.Velocity * m_Settings.GlobalFrictionCoeff * elapsedTime.Seconds();
			}
		}
	}

	void Scene::UpdateTransforms(Timestep elapsedTime)
	{
		// Transforms
		{
			auto group = m_Registry.group<Transform3DComponent, PhysicsComponent>();
			for (auto entity : group)
			{
				auto [transformComponent, physicsComponent] = group.get<Transform3DComponent, PhysicsComponent>(entity);
				transformComponent.Position += physicsComponent.Velocity * elapsedTime.Seconds();
			}
		}
	}

	void Scene::InstanciateScript(Entity entity, NativeScriptComponent& nsc)
	{
		nsc.ScriptInstance = nsc.InstanciateScript();
		nsc.ScriptInstance->m_Entity = entity;
		nsc.ScriptInstance->OnCreate();
	}

	void Scene::DestroyScript(NativeScriptComponent& nsc)
	{
		nsc.ScriptInstance->OnDestroy();
		delete nsc.ScriptInstance;
	}
}
