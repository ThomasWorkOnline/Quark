#pragma once

#include "../Events/Event.h"

namespace Quark {

	class Layer
	{
	public:
		virtual ~Layer() = default;

	protected:
		virtual void OnUpdate(float elapsedTime) {}
		virtual void OnEvent(Event& e) {}

	private:
		friend class Application;
	};
}
