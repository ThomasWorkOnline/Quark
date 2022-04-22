#pragma once

#include "Quark/Core/Core.h"
#include "Quark/Events/Event.h"

namespace Quark {

	class Layer
	{
	public:
		virtual ~Layer() = default;

		virtual void OnUpdate(Timestep elapsedTime) {}
		virtual void OnEvent(Event& e) {}
	};
}
