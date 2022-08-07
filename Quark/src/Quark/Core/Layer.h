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

		virtual void OnEvent(Event& e) {}
		virtual void OnRender() {}
		virtual void OnUpdate(Timestep elapsedTime) {}

		Window* GetWindow() const;
		Application* GetApplication() const { return m_Application; }

	private:
		Application* m_Application;
	};
}
