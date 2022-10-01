#pragma once

#include "Quark/Core/Core.h"
#include "Components.h"

#include <entt/entt.hpp>

namespace Quark {

	class Scene;

	class Entity
	{
	public:
		Entity() = default;
		Entity(entt::entity entity, Scene* scene);

		template<typename Component, typename... Args>
		Component&                    AddComponent(Args&&... args);

		template<typename Component>
		void                          RemoveComponent();

		template<typename Script>
		NativeScriptComponent&        AddNativeScript();

		template<typename Component>
		bool                          HasComponent() const;

		template<typename Component>
		Component&                    GetComponent();

		template<typename Component>
		const Component&              GetComponent() const;

		const Scene&                  GetScene() const { return *m_Scene; }
		Scene&                        GetScene() { return *m_Scene; }

		bool operator==(Entity other) const { return m_Entity == other; }
		bool operator!=(Entity other) const { return m_Entity != other; }

		operator bool()               const { return m_Entity != entt::null; }

		operator entt::entity()       const { return m_Entity; }
		operator uint32_t()           const { return (uint32_t)m_Entity; }

	private:
		void InstanciateScript(NativeScriptComponent& nsc);
		void ShutdownScript(NativeScriptComponent& nsc);

	private:
		entt::entity m_Entity = { entt::null };
		Scene* m_Scene = nullptr;
	};
}

#include "Entity.inl"
