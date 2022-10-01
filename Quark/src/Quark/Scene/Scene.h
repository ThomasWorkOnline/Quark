#pragma once

#include "Quark/Core/Core.h"
#include "Quark/Core/Timestep.h"
#include "Quark/Event/Event.h"

#include "Entity.h"

#include <entt/entt.hpp>

namespace Quark {

	struct SceneSettings
	{
		Float GlobalFrictionCoeff = 0.0f;
	};

	class Scene
	{
	public:
		Scene() = default;
		virtual ~Scene();

		void                  OnEvent(Event& e);
		void                  OnUpdate(Timestep elapsedTime);

		Entity                CreateEntity();
		void                  DeleteEntity(Entity entity);

		const SceneSettings&  GetSettings() const { return m_Settings; }
		SceneSettings&        GetSettings() { return m_Settings; }

		const entt::registry& GetRegistry() const { return m_Registry; }
		entt::registry&       GetRegistry() { return m_Registry; }

	protected:
		void UpdateNativeScripts(Timestep elapsedTime);
		void UpdatePhysicsBodies(Timestep elapsedTime);
		void UpdateTransforms(Timestep elapsedTime);

		template<typename Component>
		inline void OnComponentAdded(Entity entity, Component& c)
		{
		}

		template<typename Component>
		inline void OnComponentRemove(Entity entity, Component& c)
		{
		}

	private:
		void InstanciateScript(Entity entity, NativeScriptComponent& nsc);
		void DestroyScript(NativeScriptComponent& nsc);

	protected:
		entt::registry m_Registry;
		SceneSettings m_Settings;

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
	inline void Scene::OnComponentAdded(Entity entity, NativeScriptComponent& nsc)
	{
		InstanciateScript(entity, nsc);
	}

	template<>
	inline void Scene::OnComponentRemove(Entity entity, NativeScriptComponent& nsc)
	{
		DestroyScript(nsc);
	}
}
