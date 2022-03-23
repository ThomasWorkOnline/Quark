#include "EasingCurves.h"

namespace Quark {

	float EaseInOut(float x)
	{
		return (glm::sin((x - 0.5f) * glm::pi<float>()) + 1.0f) * 0.5f;
	}
}
