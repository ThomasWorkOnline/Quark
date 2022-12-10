#include "qkpch.h"
#include "Scene.h"

#include "Components.h"
#include "NativeScriptEntity.h"

namespace Quark {

	void Scene::OnEvent(Event& e)
	{
		// Events on native scripts
		{
			auto view = m_Registry.view<NativeScriptComponent>();
			for (auto entity : view)
			{
				auto& nsc = view.get<NativeScriptComponent>(entity);
				nsc.OnEvent(e);
			}
		}
	}

	void Scene::OnPlay()
	{
		InstanciateScript = [](Entity entity, NativeScriptComponent& nsc)
		{
			nsc.ScriptInstance = nsc.InstanciateScript();
			nsc.ScriptInstance->m_Entity = entity;
			nsc.OnCreate();
		};

		DestroyScript = [](NativeScriptComponent& nsc)
		{
			nsc.OnDestroy();
		};

		// Instanciate native scripts
		{
			auto view = m_Registry.view<NativeScriptComponent>();
			for (auto entity : view)
			{
				auto& nsc = view.get<NativeScriptComponent>(entity);

				nsc.ScriptInstance = nsc.InstanciateScript();
				nsc.ScriptInstance->m_Entity = { entity, this };
				nsc.OnCreate();
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
				nsc.OnDestroy();
			}
		}

		m_Registry.clear();
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

	Entity Scene::CreatePrimaryCamera()
	{
		auto entity = CreateEntity();
		entity.AddComponent<Transform3DComponent>();
		entity.AddComponent<PhysicsComponent>();
		entity.AddComponent<CameraComponent>().Camera.SetPerspective(90.0f);
		return entity;
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
				nsComponent.OnUpdate(elapsedTime);
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
				physicsComponent.Velocity -= physicsComponent.Velocity * physicsComponent.DragCoeff * m_Settings.AirDensity * elapsedTime.Seconds();
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
