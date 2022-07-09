#pragma once

#include "Quark/Core/Core.h"
#include "Quark/Core/Window.h"
#include "Quark/Events/Event.h"

namespace Quark {

	class Layer
	{
	public:
		virtual ~Layer() = default;

		virtual void OnEvent(Event& e) {}
		virtual void OnRender() {}
		virtual void OnUpdate(Timestep elapsedTime) {}

		const Window& GetWindow() const;
		Window& GetWindow();
	};
}
