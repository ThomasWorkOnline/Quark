#pragma once

#include "Quark/Core/Core.h"
#include "Quark/Event/Event.h"

#include "Entity.h"

namespace Quark {

	class NativeScriptEntity
	{
	public:
		virtual ~NativeScriptEntity() = default;

		virtual void OnCreate() {}
		virtual void OnDestroy() {}

		virtual void OnEvent(Event& e) {}
		virtual void OnUpdate(Timestep elapsedTime) {}

		template<typename Component>
		Component&   GetComponent() { return m_Entity.GetComponent<Component>(); }

		operator Entity()       const { return m_Entity; }

		operator entt::entity() const { return m_Entity; }
		operator uint32_t()     const { return (uint32_t)m_Entity; }

	private:
		Entity m_Entity;

		friend class Scene;
	};
}
