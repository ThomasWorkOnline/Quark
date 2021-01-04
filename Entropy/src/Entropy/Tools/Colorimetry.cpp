#include "Colorimetry.h"

#include <cmath>

namespace Entropy {

	glm::vec4 EncodeSRGB(const glm::vec4& color)
	{
		constexpr float gamma = 2.2f;
		return glm::vec4(
			pow(color.x, gamma),
			pow(color.y, gamma),
			pow(color.z, gamma),
			pow(color.w, gamma));
	}

	glm::vec4 DecodeSRGB(const glm::vec4& color)
	{
		constexpr float gamma = 2.2f;
		return glm::vec4(
			pow(color.x, 1.0f / gamma),
			pow(color.y, 1.0f / gamma),
			pow(color.z, 1.0f / gamma),
			pow(color.w, 1.0f / gamma));
	}
}
