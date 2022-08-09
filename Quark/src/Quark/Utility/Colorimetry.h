#pragma once

#include "Quark/Core/Core.h"

namespace Quark {

	// Converting between SRGB and linear colors
	Vec4 EncodeSRGB(const Vec4& color);
	Vec4 DecodeSRGB(const Vec4& color);
}
