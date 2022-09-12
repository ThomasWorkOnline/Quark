#pragma once

#include <cstdint>

namespace Quark {

	enum class ColorDataFormat
	{
		None = 0,

		RGB8,
		RGB16,
		RGB32,

		RGBA8,
		RGBA16,

		// sRGB formats
		RGB8_SRGB,
		RGBA8_SRGB,

		BGR8_SRGB,
		BGRA8_SRGB,

		// HDR formats
		RGB16f,
		RGB32f,
		RGBA16f,
		RGBA32f,

		Red8,

		// Depth stencil formats
		Depth24,
		Depth24Stencil8
	};

	size_t GetPixelFormatSize(ColorDataFormat format);

	bool IsColorFormatAlpha(ColorDataFormat format);
	bool IsPixel4BytesAligned(ColorDataFormat format);
}
