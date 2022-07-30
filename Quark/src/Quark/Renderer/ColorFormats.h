#pragma once

namespace Quark {

	enum class ColorDataFormat
	{
		None = 0,

		RGB8,
		RGB10,
		RGB12,
		RGB16,
		RGB32,

		RGBA8,
		RGBA12,
		RGBA16,

		// sRGB formats
		RGB8_SRGB,
		RGBA8_SRGB,

		BRG8_SRGB,
		BRGA8_SRGB,

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

	constexpr size_t GetPixelFormatSize(ColorDataFormat format)
	{
		switch (format)
		{
			case ColorDataFormat::RGB8:            return 3;
			case ColorDataFormat::RGB10:           return 4;
			case ColorDataFormat::RGB12:           return 5;
			case ColorDataFormat::RGB16:           return 6;
			case ColorDataFormat::RGB32:           return 12;
			case ColorDataFormat::RGBA8:           return 4;
			case ColorDataFormat::RGBA12:          return 6;
			case ColorDataFormat::RGBA16:          return 8;
			case ColorDataFormat::RGB8_SRGB:       return 3;
			case ColorDataFormat::RGBA8_SRGB:      return 4;
			case ColorDataFormat::BRG8_SRGB:       return 3;
			case ColorDataFormat::BRGA8_SRGB:      return 4;
			case ColorDataFormat::Red8:            return 1;
			case ColorDataFormat::RGB16f:          return 6;
			case ColorDataFormat::RGB32f:          return 12;
			case ColorDataFormat::RGBA16f:         return 8;
			case ColorDataFormat::RGBA32f:         return 16;
			case ColorDataFormat::Depth24:         return 3;
			case ColorDataFormat::Depth24Stencil8: return 4;
			default:
				QK_CORE_FATAL("Invalid internal texture format");
				return 0;
		}
	}

	constexpr bool IsColorFormatAlpha(ColorDataFormat format)
	{
		switch (format)
		{
			case ColorDataFormat::RGBA8:
			case ColorDataFormat::RGBA12:
			case ColorDataFormat::RGBA16:
			case ColorDataFormat::RGBA8_SRGB:
			case ColorDataFormat::BRGA8_SRGB:
			case ColorDataFormat::RGBA16f:
			case ColorDataFormat::RGBA32f:
				return true;
			default:
				return false;
		}
	}
}
