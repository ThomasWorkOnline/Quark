#pragma once

#include "Quark/Core/Core.h"
#include "Quark/Event/Event.h"

#include "Scene.h"
#include "Entity.h"

namespace Quark {

	class NativeScriptEntity
	{
	public:
		virtual ~NativeScriptEntity() = default;

		void                    OnCreate() {}
		void                    OnDestroy() {}

		void                    OnEvent(Event& e) {}
		void                    OnUpdate(Timestep elapsedTime) {}

		template<typename Component, typename... Args>
		Component&              AddComponent(Args&&... args) { return m_Entity.AddComponent<Component>(std::forward<Args>(args)...); }

		template<typename Component>
		void                    RemoveComponent() { m_Entity.RemoveComponent<Component>(); }

		template<typename Component>
		bool                    HasComponent() const { return m_Entity.HasComponent<Component>(); }

		template<typename Component>
		Component&              GetComponent() { return m_Entity.GetComponent<Component>(); }

		const Scene&            GetScene() const { return m_Entity.GetScene(); }
		Scene&                  GetScene() { return m_Entity.GetScene(); }

		operator Entity()       const { return m_Entity; }

		operator entt::entity() const { return m_Entity; }
		operator uint32_t()     const { return (uint32_t)m_Entity; }

	private:
		Entity m_Entity;

		friend class Scene;
	};
}
