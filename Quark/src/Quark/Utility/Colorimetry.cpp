#include "qkpch.h"
#include "Colorimetry.h"

namespace Quark {

	Vec4f EncodeSRGB(const Vec4f& color)
	{
		static constexpr Vec4f::value_type gamma = 1.0f / 2.2f;
		return Vec4f(
			glm::pow(color.r, gamma),
			glm::pow(color.g, gamma),
			glm::pow(color.b, gamma),
			color.a);
	}

	Vec4f DecodeSRGB(const Vec4f& color)
	{
		static constexpr Vec4f::value_type gamma = 2.2f;
		return Vec4f(
			glm::pow(color.r, gamma),
			glm::pow(color.g, gamma),
			glm::pow(color.b, gamma),
			color.a);
	}
}
