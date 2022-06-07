#pragma once

#include "Quark/Core/Core.h"
#include "Quark/Core/Timestep.h"
#include "Quark/Scene/Entity.h"

#include <entt/entt.hpp>

namespace Quark {

	class Entity;

	class Scene
	{
	public:
		void OnUpdate(Timestep elapsedTime);

		Entity CreateEntity();
		void DeleteEntity(Entity entity);

		void SetPrimaryCamera(Entity cameraEntity);

		const entt::registry& GetRegistryRaw() const { return m_Registry; }
		entt::registry& GetRegistryRaw() { return m_Registry; }

		static Ref<Scene> Create();

	private:
		entt::registry m_Registry;
		Entity m_CameraEntity;

		friend class Entity;
		friend class SceneRenderer;
	};
}
