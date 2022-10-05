#include "qkpch.h"
#include "Scene.h"

#include "Components.h"
#include "NativeScriptEntity.h"

namespace Quark {

	void Scene::OnEvent(Event& e)
	{
		QK_CORE_ASSERT(InstanciateScript, "OnEvent() was called on a stopped scene! Make sure to call OnPlay() before sending events");

		// Events on native scripts
		{
			auto view = m_Registry.view<NativeScriptComponent>();
			for (auto entity : view)
			{
				auto& nsc = view.get<NativeScriptComponent>(entity);
				nsc.OnEvent(nsc.ScriptInstance, e);
			}
		}
	}

	void Scene::OnPlay()
	{
		InstanciateScript = [](Entity entity, NativeScriptComponent& nsc)
		{
			nsc.ScriptInstance = nsc.InstanciateScript();
			nsc.ScriptInstance->m_Entity = entity;
			nsc.OnCreate(nsc);
		};

		DestroyScript = [](NativeScriptComponent& nsc)
		{
			nsc.OnDestroy(nsc);
		};

		// Instanciate native scripts
		{
			auto view = m_Registry.view<NativeScriptComponent>();
			for (auto entity : view)
			{
				auto& nsc = view.get<NativeScriptComponent>(entity);

				nsc.ScriptInstance = nsc.InstanciateScript();
				nsc.ScriptInstance->m_Entity = { entity, this };
				nsc.OnCreate(nsc);
			}
		}
	}

	void Scene::OnStop()
	{
		// Destroy native scripts
		{
			auto view = m_Registry.view<NativeScriptComponent>();
			for (auto entity : view)
			{
				auto& nsc = view.get<NativeScriptComponent>(entity);
				DestroyScript(nsc);
			}
		}

		m_Registry.clear();
	}

	void Scene::OnUpdate(Timestep elapsedTime)
	{
		QK_CORE_ASSERT(InstanciateScript, "OnUpdate() was called on a stopped scene! Make sure to call OnPlay() before updating the scene");

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
				nsComponent.OnUpdate(nsComponent.ScriptInstance, elapsedTime);
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
}
