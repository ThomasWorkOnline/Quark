#pragma once

#include "Quark/Core/Core.h"
#include "Quark/Core/Timestep.h"
#include "Quark/Event/Event.h"

#include "Entity.h"

#include <entt/entt.hpp>

namespace Quark {

	struct SceneSettings
	{
		Float AirDensity = 1.225f;
	};

	enum class SceneStatus
	{
		Stopped = 0,
		Playing = 1
	};

	class Scene
	{
	public:
		void                  OnEvent(Event& e);
		void                  OnUpdate(Timestep elapsedTime);

		void                  OnPlay();
		void                  OnStop();

		Entity                CreateEntity();
		Entity                CreatePrimaryCamera();
		Entity                CreateEmptyEntity();
		void                  DestroyEntity(Entity entity);

		const SceneSettings&  GetSettings() const { return m_Settings; }
		SceneSettings&        GetSettings() { return m_Settings; }

		const entt::registry& GetRegistry() const { return m_Registry; }
		entt::registry&       GetRegistry() { return m_Registry; }

	private:
		template<typename Script>
		static inline void InstanciateScript(Entity entity, Script& sc) {}

		template<typename Script>
		static inline void DestroyScript(Script& sc) {}

		template<typename Component>
		inline void OnComponentAdded(Entity entity, Component& c) {}

		template<typename Component>
		inline void OnComponentRemove(Entity entity, Component& c) {}

		void UpdateScripts(Timestep elapsedTime);
		void UpdatePhysicsBodies(Timestep elapsedTime);
		void UpdateTransforms(Timestep elapsedTime);

	private:
		entt::registry m_Registry;
		SceneSettings m_Settings;
		SceneStatus m_Status{};

		friend class Entity;
	};
}

#include "Entity.inl"
#include "Components.h"

namespace Quark {

	///////////////////////////////////////////////////////////////////////////////////////////////////////
	// Component template specialization
	//

	template<>
	void Scene::OnComponentAdded(Entity entity, ScriptComponent& sc);

	template<>
	void Scene::OnComponentRemove(Entity entity, ScriptComponent& sc);

	template<>
	void Scene::OnComponentAdded(Entity entity, NativeScriptComponent& nsc);

	template<>
	void Scene::OnComponentRemove(Entity entity, NativeScriptComponent& nsc);

	template<>
	void Scene::OnComponentAdded(Entity entity, CameraComponent& cc);
}
