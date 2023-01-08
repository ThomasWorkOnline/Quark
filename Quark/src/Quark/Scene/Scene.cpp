#include "qkpch.h"
#include "Scene.h"

#include "Components.h"
#include "NativeScriptEntity.h"

#include "Quark/Scripting/ScriptRuntime.h"

namespace Quark {

	template<>
	void Scene::InstanciateScript(Entity entity, ScriptComponent& sc)
	{
		sc.ScriptInstance.Instanciate();

		if (sc.OnCreate)
			sc.OnCreate.Invoke(sc.ScriptInstance);
	}

	template<>
	void Scene::DestroyScript(ScriptComponent& sc)
	{
		if (sc.OnDestroy)
			sc.OnDestroy.Invoke(sc.ScriptInstance);

		// TODO: destroy the instance
		sc.ScriptInstance = nullptr;
	}

	template<>
	void Scene::InstanciateScript(Entity entity, NativeScriptComponent& nsc)
	{
		nsc.ScriptInstance = nsc.InstanciateScript();
		nsc.ScriptInstance->m_Entity = entity;
		nsc.OnCreate(nsc.ScriptInstance);
	}

	template<>
	void Scene::DestroyScript(NativeScriptComponent& nsc)
	{
		nsc.OnDestroy(nsc.ScriptInstance);
		nsc.ScriptInstance = nullptr;
	}

	void Scene::OnEvent(Event& e)
	{
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
		m_Status = SceneStatus::Playing;

		ScriptRuntime::OnRuntimeStart(this);

		// Instanciate native scripts
		{
			auto view = m_Registry.view<NativeScriptComponent>();
			for (auto entity : view)
			{
				auto& nsc = view.get<NativeScriptComponent>(entity);
				Entity e = { entity, this };
				InstanciateScript(e, nsc);
			}
		}

		// Instanciate C# scripts
		{
			auto view = m_Registry.view<ScriptComponent>();
			for (auto entity : view)
			{
				auto& sc = view.get<ScriptComponent>(entity);
				Entity e = { entity, this };
				InstanciateScript(e, sc);
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

		// Destroy C# scripts
		{
			auto view = m_Registry.view<ScriptComponent>();
			for (auto entity : view)
			{
				auto& sc = view.get<ScriptComponent>(entity);
				DestroyScript(sc);
			}
		}

		ScriptRuntime::OnRuntimeStop();

		m_Status = SceneStatus::Stopped;
		m_Registry.clear();
	}

	void Scene::OnUpdate(Timestep elapsedTime)
	{
		UpdateScripts(elapsedTime);
		UpdatePhysicsBodies(elapsedTime);
		UpdateTransforms(elapsedTime);
	}

	Entity Scene::CreateEntity()
	{
		Entity entity = CreateEmptyEntity();
		entity.AddComponent<Transform3DComponent>();
		entity.AddComponent<PhysicsComponent>();
		return entity;
	}

	Entity Scene::CreatePrimaryCamera()
	{
		Entity entity = CreateEntity();
		entity.AddComponent<CameraComponent>().Camera.SetPerspective(90.0f);
		return entity;
	}

	Entity Scene::CreateEmptyEntity()
	{
		return { m_Registry.create(), this };
	}

	void Scene::DestroyEntity(Entity entity)
	{
		m_Registry.destroy(entity);
	}

	void Scene::UpdateScripts(Timestep elapsedTime)
	{
		// Native scripts
		{
			auto view = m_Registry.view<NativeScriptComponent>();
			for (auto entity : view)
			{
				auto& nsc = view.get<NativeScriptComponent>(entity);
				nsc.OnUpdate(nsc.ScriptInstance, elapsedTime);
			}
		}

		// C# scripts
		{
			float ts = (float)elapsedTime.Seconds();
			void* params[] = { &ts };

			auto view = m_Registry.view<ScriptComponent>();
			for (auto entity : view)
			{
				auto& sc = view.get<ScriptComponent>(entity);

				if (sc.OnUpdate)
					sc.OnUpdate.Invoke(sc.ScriptInstance, params);
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

#include "Quark/Renderer/Renderer.h"

namespace Quark {

	template<>
	void Scene::OnComponentAdded(Entity entity, ScriptComponent& sc)
	{
		if (m_Status != SceneStatus::Playing)
			return;

		InstanciateScript(entity, sc);
	}

	template<>
	void Scene::OnComponentRemove(Entity entity, ScriptComponent& sc)
	{
		if (m_Status != SceneStatus::Playing)
			return;

		DestroyScript(sc);
	}

	template<>
	void Scene::OnComponentAdded(Entity entity, NativeScriptComponent& nsc)
	{
		if (m_Status != SceneStatus::Playing)
			return;

		InstanciateScript(entity, nsc);
	}

	template<>
	void Scene::OnComponentRemove(Entity entity, NativeScriptComponent& nsc)
	{
		if (m_Status != SceneStatus::Playing)
			return;

		DestroyScript(nsc);
	}

	template<>
	void Scene::OnComponentAdded(Entity entity, CameraComponent& cc)
	{
		auto extent = Renderer::GetViewportExtent();
		cc.Camera.Resize((float)extent.Width, (float)extent.Height);
	}
}
