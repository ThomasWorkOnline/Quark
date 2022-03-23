#include "Colorimetry.h"

namespace Quark {

	glm::vec4 EncodeSRGB(const glm::vec4& color)
	{
		static constexpr float gamma = 1.0f / 2.2f;
		return glm::vec4(
			pow(color.r, gamma),
			pow(color.g, gamma),
			pow(color.b, gamma),
			color.a);
	}

	glm::vec4 DecodeSRGB(const glm::vec4& color)
	{
		static constexpr float gamma = 2.2f;
		return glm::vec4(
			pow(color.r, gamma),
			pow(color.g, gamma),
			pow(color.b, gamma),
			color.a);
	}
}
