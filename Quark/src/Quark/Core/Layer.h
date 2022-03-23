#pragma once

#include "Quark/Events/Event.h"

namespace Quark {

	class Layer
	{
	public:
		virtual ~Layer() = default;

	protected:
		virtual void OnUpdate(Timestep elapsedTime) {}
		virtual void OnEvent(Event& e) {}

	private:
		friend class Application;
	};
}
