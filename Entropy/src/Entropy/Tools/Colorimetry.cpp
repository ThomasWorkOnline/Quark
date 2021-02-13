#include "Colorimetry.h"

namespace Entropy {

	glm::vec4 EncodeSRGB(const glm::vec4& color)
	{
		constexpr float gamma = 2.2f;
		return glm::vec4(
			pow(color.r, gamma),
			pow(color.g, gamma),
			pow(color.b, gamma),
			color.a);
	}

	glm::vec4 DecodeSRGB(const glm::vec4& color)
	{
		constexpr float gamma = 2.2f;
		return glm::vec4(
			pow(color.r, 1.0f / gamma),
			pow(color.g, 1.0f / gamma),
			pow(color.b, 1.0f / gamma),
			color.a);
	}
}
