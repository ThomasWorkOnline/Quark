#pragma once

#include "Quark/Core/Core.h"
#include "Quark/Core/Timestep.h"

#include "Quark/Event/Event.h"
#include "Quark/Scene/Entity.h"

#include <entt/entt.hpp>

namespace Quark {

	struct SceneSettings
	{
		Float GlobalFrictionCoeff = 4.0f;
	};

	class Scene
	{
	public:
		Scene() = default;
		~Scene();

		void OnUpdate(Timestep elapsedTime);
		void OnEvent(Event& e);

		Entity CreateEntity();
		Entity CreatePrimaryCamera();

		void DeleteEntity(Entity entity);
		void SetPrimaryCamera(Entity cameraEntity);

		bool HasPrimaryCamera() const { return (bool)m_PrimaryCameraEntity; }

		const SceneSettings& GetSettings() const { return m_Settings; }
		SceneSettings& GetSettings() { return m_Settings; }

		const entt::registry& GetRegistry() const { return m_Registry; }
		entt::registry& GetRegistry() { return m_Registry; }

		operator const entt::registry& () const { return m_Registry; }
		operator entt::registry&() { return m_Registry; }

		static Ref<Scene> Create();

	private:
		template<typename Component>
		void OnComponentAdded(Entity entity, Component& c);

		template<typename Component>
		void OnComponentRemove(Entity entity, Component& c);

	private:
		entt::registry m_Registry;
		Entity m_PrimaryCameraEntity;

		SceneSettings m_Settings;

		friend class Entity;
		friend class SceneRenderer;
	};
}
