#pragma once

#include "Entity.h"

#include <typeinfo>
#include <type_traits>

namespace Quark {

	template<typename Component, typename... Args>
	inline Component& Entity::AddComponent(Args&&... args)
	{
		static_assert(!std::is_same_v<Component, NativeScriptComponent>,
			"Invalid usage. Please, use the AddNativeScript() method when adding a native script component.");

		QK_CORE_ASSERT(!HasComponent<Component>(), "Entity already has a {0}!", typeid(Component).name());
		auto& component = m_Scene->m_Registry.emplace<Component>(m_Entity, std::forward<Args>(args)...);
		m_Scene->OnComponentAdded<Component>(*this, component);
		return component;
	}

	template<typename Component>
	inline void Entity::RemoveComponent()
	{
		QK_CORE_ASSERT(HasComponent<Component>(), "Entity does not have a {0}!", typeid(Component).name());
		m_Scene->OnComponentRemove<Component>(*this, GetComponent<Component>());
		m_Scene->m_Registry.remove<Component>(m_Entity);
	}

	template<typename Script>
	inline NativeScriptComponent& Entity::AddNativeScript()
	{
		static_assert(std::is_base_of_v<NativeScriptEntity, Script>,
			"Invalid usage. Type is not a sub-type of NativeScriptEntity. Please, use the AddComponent() method when adding a non-scriptable component.");

		QK_CORE_ASSERT(!HasComponent<NativeScriptComponent>(), "Entity already has a NativeScriptComponent installed!");
		auto& script = m_Scene->m_Registry.emplace<NativeScriptComponent>(m_Entity).Bind<Script>();
		m_Scene->OnComponentAdded<NativeScriptComponent>(*this, script);
		return script;
	}

	template<typename Component>
	inline bool Entity::HasComponent() const
	{
		return m_Scene->m_Registry.any_of<Component>(m_Entity);
	}

	template<typename Component>
	inline Component& Entity::GetComponent()
	{
		QK_CORE_ASSERT(HasComponent<Component>(), "Entity does not have a {0}!", typeid(Component).name());
		return m_Scene->m_Registry.get<Component>(m_Entity);
	}

	template<typename Component>
	inline const Component& Entity::GetComponent() const
	{
		QK_CORE_ASSERT(HasComponent<Component>(), "Entity does not have a {0}!", typeid(Component).name());
		return m_Scene->m_Registry.get<Component>(m_Entity);
	}
}
