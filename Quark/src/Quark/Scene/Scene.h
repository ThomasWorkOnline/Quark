#pragma once

#include "Quark/Core/Core.h"
#include "Quark/Core/Timestep.h"
#include "Quark/Events/Event.h"
#include "Quark/Scene/Entity.h"

#include <entt/entt.hpp>

namespace Quark {

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

		const entt::registry& GetRegistryRaw() const { return m_Registry; }
		entt::registry& GetRegistryRaw() { return m_Registry; }

		operator const entt::registry& () const { return m_Registry; }
		operator entt::registry&() { return m_Registry; }

		static Ref<Scene> Create();

	private:
		entt::registry m_Registry;
		Entity m_CameraEntity;

		friend class SceneRenderer;
	};
}
