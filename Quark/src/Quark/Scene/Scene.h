#pragma once

#include "Quark/Core/Core.h"
#include "Quark/Core/Timestep.h"

#include <entt/entt.hpp>

namespace Quark {

	class Entity;

	class Scene
	{
	public:
		void OnUpdate(Timestep elapsedTime);
		void OnRender();

		Entity CreateEntity();
		void DeleteEntity(Entity entity);

		const entt::registry& GetRegistryRaw() const { return m_Registry; }
		entt::registry& GetRegistryRaw() { return m_Registry; }

	private:
		entt::registry m_Registry;

		friend class Entity;
	};
}
