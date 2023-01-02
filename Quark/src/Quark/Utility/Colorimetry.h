#pragma once

#include "Quark/Core/Core.h"

namespace Quark {

	// Converting between SRGB and linear colors
	Vec4f EncodeSRGB(const Vec4f& color);
	Vec4f DecodeSRGB(const Vec4f& color);
}
