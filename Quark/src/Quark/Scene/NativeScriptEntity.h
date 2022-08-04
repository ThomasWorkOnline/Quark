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

		virtual void OnUpdate(Timestep elapsedTime) {}
		virtual void OnEvent(Event& e) {}

		template<typename T>
		T& GetComponent() { return m_Entity.GetComponent<T>(); }

		operator entt::entity() const { return m_Entity; }
		operator uint32_t() const { return (uint32_t)m_Entity; }

	private:
		Entity m_Entity;

		friend struct NativeScriptComponent;
	};
}
