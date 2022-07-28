#pragma once

#include "Quark/Core/Core.h"
#include "Components.h"

#include <entt/entt.hpp>
#include <typeinfo>

namespace Quark {

	class Scene;

	class Entity
	{
	public:
		Entity() = default;
		Entity(entt::entity entity, Scene* scene);

		template<typename Component, typename... Args>
		Component& AddComponent(Args&&... args);

		template<typename Component>
		void RemoveComponent();

		template<typename Script>
		NativeScriptComponent& AddNativeScript();

		template<typename Component>
		bool HasComponent() const;

		template<typename Component>
		Component& GetComponent();

		template<typename Component>
		const Component& GetComponent() const;

		const Scene& GetScene() const { return *m_Scene; }
		Scene& GetScene() { return *m_Scene; }

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

#include "Scene.h"
#include "Components.h"

namespace Quark {

	template<typename Component, typename... Args>
	inline Component& Entity::AddComponent(Args&&... args)
	{
		static_assert(!std::is_same_v<Component, NativeScriptComponent>,
			"Invalid usage. Please, use the AddNativeScript() method when adding a native script component");

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
