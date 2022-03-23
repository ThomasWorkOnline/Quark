#pragma once

#include <ostream>
#include <utility>

#include "KeyCodes.h"
#include "MouseCodes.h"

namespace Quark {

	class Input
	{
	public:
		static bool IsKeyPressed(KeyCode key);

		static bool IsMouseButtonPressed(MouseCode button);
		static std::pair<float, float> GetMousePosition();
		static float GetMouseX();
		static float GetMouseY();
	};
}
