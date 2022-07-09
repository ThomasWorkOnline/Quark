#pragma once

#include "Quark/Core/Core.h"

#include <entt/entt.hpp>

namespace Quark {

	class Entity
	{
	public:
		Entity() = default;
		Entity(entt::entity entity, entt::registry* registry);

		template<typename Component, typename... Args>
		inline Component& AddComponent(Args&&... args);

		template<typename Component>
		void RemoveComponent();

		template<typename Component>
		bool HasComponent() const;

		template<typename Component>
		Component& GetComponent();

		template<typename Component>
		const Component& GetComponent() const;

		bool operator==(Entity other) const { return m_Entity == other; }
		bool operator!=(Entity other) const { return m_Entity != other; }

		operator bool() const { return m_Entity != entt::null; }

		operator entt::entity() const { return m_Entity; }
		operator uint32_t() const { return (uint32_t)m_Entity; }

	private:
		entt::entity m_Entity = { entt::null };
		entt::registry* m_Registry = nullptr;
	};

	template<typename Component, typename... Args>
	inline Component& Entity::AddComponent(Args&&... args)
	{
		QK_CORE_ASSERT(!HasComponent<Component>(), "Entity already has component!");
		return m_Registry->emplace<Component>(m_Entity, std::forward<Args>(args)...);
	}

	template<typename Component>
	inline void Entity::RemoveComponent()
	{
		QK_CORE_ASSERT(HasComponent<Component>(), "Entity does not have component!");
		m_Registry->remove<Component>(m_Entity);
	}

	template<typename Component>
	inline bool Entity::HasComponent() const
	{
		return m_Registry->any_of<Component>(m_Entity);
	}

	template<typename Component>
	inline Component& Entity::GetComponent()
	{
		QK_CORE_ASSERT(HasComponent<Component>(), "Entity does not have component!");
		return m_Registry->get<Component>(m_Entity);
	}

	template<typename Component>
	inline const Component& Entity::GetComponent() const
	{
		QK_CORE_ASSERT(HasComponent<Component>(), "Entity does not have component!");
		return m_Registry->get<Component>(m_Entity);
	}
}
