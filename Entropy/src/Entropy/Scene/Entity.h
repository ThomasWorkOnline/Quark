#pragma once

#include "../Core/Core.h"

#include "Scene.h"

#include <entt.hpp>

namespace Entropy {

	class Entity
	{
	public:
		Entity() = default;
		Entity(entt::entity entity, Scene* scene);

		template<typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			NT_ASSERT(!HasComponent<T>(), "Entity already has component!");
			T& component = m_Scene->m_Registry.emplace<T>(m_Entity, std::forward<Args>(args)...);
			return component;
		}

		template<typename T>
		void RemoveComponent()
		{
			NT_ASSERT(HasComponent<T>(), "Entity does not have component!");
			m_Scene->m_Registry.remove<T>(m_Entity);
		}

		template<typename T>
		bool HasComponent()
		{
			return m_Scene->m_Registry.has<T>(m_Entity);
		}

		template<typename T>
		T& GetComponent()
		{
			NT_ASSERT(HasComponent<T>(), "Entity does not have component!");
			return m_Scene->m_Registry.get<T>(m_Entity);
		}

		operator bool() const { return m_Entity != entt::null; }
		operator entt::entity() const { return m_Entity; }

	private:
		entt::entity m_Entity = { entt::null };
		Scene* m_Scene = nullptr;
	};
}
