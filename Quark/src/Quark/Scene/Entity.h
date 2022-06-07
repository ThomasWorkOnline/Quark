#pragma once

#include "Quark/Core/Core.h"

#include <entt/entt.hpp>

namespace Quark {

	class Entity
	{
	public:
		Entity() = default;
		Entity(entt::entity entity, entt::registry* registry);

		template<typename T, typename... Args>
		inline T& AddComponent(Args&&... args);

		template<typename T>
		void RemoveComponent();

		template<typename T>
		bool HasComponent() const;

		template<typename T>
		T& GetComponent();

		template<typename T>
		const T& GetComponent() const;

		bool operator==(Entity other) const { return m_Entity == other; }
		bool operator!=(Entity other) const { return m_Entity != other; }

		operator bool() const { return m_Entity != entt::null; }

		operator entt::entity() const { return m_Entity; }
		operator uint32_t() const { return (uint32_t)m_Entity; }

	private:
		entt::entity m_Entity = { entt::null };
		entt::registry* m_Registry = nullptr;
	};

	template<typename T, typename ...Args>
	inline T& Entity::AddComponent(Args && ...args)
	{
		QK_CORE_ASSERT(!HasComponent<T>(), "Entity already has component!");
		return m_Registry->emplace<T>(m_Entity, std::forward<Args>(args)...);
	}

	template<typename T>
	inline void Entity::RemoveComponent()
	{
		QK_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
		m_Registry->remove<T>(m_Entity);
	}

	template<typename T>
	inline bool Entity::HasComponent() const
	{
		return m_Registry->any_of<T>(m_Entity);
	}

	template<typename T>
	inline T& Entity::GetComponent()
	{
		QK_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
		return m_Registry->get<T>(m_Entity);
	}

	template<typename T>
	inline const T& Entity::GetComponent() const
	{
		QK_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
		return m_Registry->get<T>(m_Entity);
	}
}
