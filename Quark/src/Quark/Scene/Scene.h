#pragma once

#include "Quark/Core/Core.h"
#include "Quark/Core/Timestep.h"

#include "Quark/Event/Event.h"
#include "Quark/Scene/Entity.h"

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

		void OnUpdate(Timestep elapsedTime);
		void OnEvent(Event& e);

		Entity CreateEntity();
		void DeleteEntity(Entity entity);

		const SceneSettings& GetSettings() const { return m_Settings; }
		SceneSettings& GetSettings() { return m_Settings; }

		const entt::registry& GetRegistry() const { return m_Registry; }
		entt::registry& GetRegistry() { return m_Registry; }

	protected:
		template<typename Component>
		inline void OnComponentAdded(Entity entity, Component& c);

		template<typename Component>
		inline void OnComponentRemove(Entity entity, Component& c);

	protected:
		entt::registry m_Registry;
		SceneSettings m_Settings;

		friend class Entity;
	};
}
