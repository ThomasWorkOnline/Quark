#pragma once

#include "../Core/Core.h"
#include "../Core/Timestep.h"

#include <entt.hpp>

namespace Quark {

	class Entity;

	class Scene
	{
	public:
		Scene();
		~Scene();

		void OnUpdate(Timestep elapsedTime);

		Entity CreateEntity();
		void DeleteEntity(Entity entity);

		const entt::registry& GetRegistryRaw() const { return m_Registry; }
		entt::registry& GetRegistryRaw() { return m_Registry; }

	private:
		entt::registry m_Registry;

		friend class Entity;
	};
}
