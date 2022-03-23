#pragma once

#include "Quark/Core/Core.h"

#include "Scene.h"

namespace Quark {

	class Entity
	{
	public:
		Entity() = default;
		Entity(entt::entity entity, Scene& scene);

		template<typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			QK_CORE_ASSERT(!HasComponent<T>(), "Entity already has component!");
			return m_Scene->m_Registry.emplace<T>(m_Entity, std::forward<Args>(args)...);
		}

		template<typename T>
		void RemoveComponent()
		{
			QK_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
			m_Scene->m_Registry.remove<T>(m_Entity);
		}

		template<typename T>
		bool HasComponent() const
		{
			return m_Scene->m_Registry.has<T>(m_Entity);
		}

		template<typename T>
		T& GetComponent()
		{
			QK_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
			return m_Scene->m_Registry.get<T>(m_Entity);
		}

		template<typename T>
		const T& GetComponent() const
		{
			QK_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
			return m_Scene->m_Registry.get<T>(m_Entity);
		}

		bool operator==(Entity other) const { return m_Entity == other; }
		bool operator!=(Entity other) const { return m_Entity != other; }

		operator bool() const { return m_Entity != entt::null; }

		operator entt::entity() const { return m_Entity; }
		operator uint32_t() const { return (uint32_t)m_Entity; }

	private:
		entt::entity m_Entity = { entt::null };
		Scene* m_Scene = nullptr;
	};
}
