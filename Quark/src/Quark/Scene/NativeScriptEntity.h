#pragma once

#include "Quark/Core/Core.h"
#include "Quark/Events/Event.h"
#include "Quark/Events/ApplicationEvent.h"
#include "Quark/Events/KeyEvent.h"
#include "Quark/Events/MouseEvent.h"
#include "Quark/Events/WindowEvents.h"

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

		friend class Scene;
	};
}
