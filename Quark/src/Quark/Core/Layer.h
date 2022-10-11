#pragma once

#include "Quark/Core/Core.h"
#include "Quark/Core/Window.h"

#include "Quark/Event/Event.h"

namespace Quark {

	class Application;

	class Layer
	{
	public:
		Layer(Application* app);
		virtual ~Layer() = default;

		Window*      GetWindow() const;
		Application* GetApplication() const { return m_Application; }

	protected:
		virtual void OnEvent(Event& e) {}
		virtual void OnRender() {}
		virtual void OnUpdate(Timestep elapsedTime) {}

	private:
		Application* m_Application;

		friend class Application;
	};
}
