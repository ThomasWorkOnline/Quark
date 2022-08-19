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

	void Scene::DeleteEntity(Entity entity)
	{
		m_Registry.destroy(entity);
	}
}

#include "Quark/Renderer/Renderer.h"

namespace Quark {

	template<typename Component>
	void Scene::OnComponentAdded(Entity entity, Component& c)
	{
	}

	template<typename Component>
	void Scene::OnComponentRemove(Entity entity, Component& c)
	{
	}

	template<>
	void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& cc)
	{
		auto extent = Renderer::GetViewportExtent();
		cc.Camera.Resize(extent.Width, extent.Height);
	}

	template<>
	void Scene::OnComponentRemove<CameraComponent>(Entity entity, CameraComponent& cc)
	{
	}

	template<>
	void Scene::OnComponentAdded<TagComponent>(Entity entity, TagComponent& tag)
	{
	}

	template<>
	void Scene::OnComponentRemove<TagComponent>(Entity entity, TagComponent& tag)
	{
	}

	template<>
	void Scene::OnComponentRemove<Transform3DComponent>(Entity entity, Transform3DComponent& tc)
	{
	}

	template<>
	void Scene::OnComponentAdded<Transform3DComponent>(Entity entity, Transform3DComponent& tc)
	{
	}

	template<>
	void Scene::OnComponentRemove<PhysicsComponent>(Entity entity, PhysicsComponent& psc)
	{
	}

	template<>
	void Scene::OnComponentAdded<PhysicsComponent>(Entity entity, PhysicsComponent& psc)
	{
	}

	template<>
	void Scene::OnComponentRemove<MeshComponent>(Entity entity, MeshComponent& mc)
	{
	}

	template<>
	void Scene::OnComponentAdded<MeshComponent>(Entity entity, MeshComponent& mc)
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
	void Scene::OnComponentAdded<TextRendererComponent>(Entity entity, TextRendererComponent& trc)
	{
	}

	template<>
	void Scene::OnComponentRemove<TextRendererComponent>(Entity entity, TextRendererComponent& trc)
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
