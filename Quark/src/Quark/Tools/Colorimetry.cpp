#include "qkpch.h"
#include "Colorimetry.h"

namespace Quark {

	Vec4 EncodeSRGB(const Vec4& color)
	{
		static constexpr Vec4::value_type gamma = 1.0f / 2.2f;
		return Vec4(
			glm::pow(color.r, gamma),
			glm::pow(color.g, gamma),
			glm::pow(color.b, gamma),
			color.a);
	}

	Vec4 DecodeSRGB(const Vec4& color)
	{
		static constexpr Vec4::value_type gamma = 2.2f;
		return Vec4(
			glm::pow(color.r, gamma),
			glm::pow(color.g, gamma),
			glm::pow(color.b, gamma),
			color.a);
	}
}
