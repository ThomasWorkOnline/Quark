#include "EasingCurves.h"

#define _USE_MATH_DEFINES
#include <cmath>

namespace Entropy {

	float EaseInOut(float x)
	{
		return (sinf((x - 0.5f) * (float)M_PI) + 1.0f) * 0.5f;
	}
}
