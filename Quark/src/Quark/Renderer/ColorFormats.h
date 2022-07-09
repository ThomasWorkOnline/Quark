#pragma once

namespace Quark {

	enum class ColorDataFormat
	{
		None = 0,

		// Color formats
		RGB,
		RGBA,

		BGR,
		BGRA,

		Red,

		Depth
	};

	enum class InternalColorFormat
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
		SRGB8,
		SRGBA8,

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

	constexpr uint32_t GetComponentCount(ColorDataFormat format)
	{
		switch (format)
		{
			case ColorDataFormat::RGB:   return 3;
			case ColorDataFormat::RGBA:  return 4;
			case ColorDataFormat::BGR:   return 3;
			case ColorDataFormat::BGRA:  return 4;
			case ColorDataFormat::Red:   return 1;
			case ColorDataFormat::Depth: return 1;
			default:
				QK_CORE_FATAL("Invalid texture data format");
			return 0;
		}
	}

	constexpr size_t GetPixelFormatSize(InternalColorFormat format)
	{
		switch (format)
		{
			case InternalColorFormat::RGB8:            return 3;
			case InternalColorFormat::RGB10:           return 4;
			case InternalColorFormat::RGB12:           return 5;
			case InternalColorFormat::RGB16:           return 6;
			case InternalColorFormat::RGB32:           return 12;
			case InternalColorFormat::RGBA8:           return 4;
			case InternalColorFormat::RGBA12:          return 6;
			case InternalColorFormat::RGBA16:          return 8;
			case InternalColorFormat::SRGB8:           return 3;
			case InternalColorFormat::SRGBA8:          return 4;
			case InternalColorFormat::Red8:            return 1;
			case InternalColorFormat::RGB16f:          return 6;
			case InternalColorFormat::RGB32f:          return 12;
			case InternalColorFormat::RGBA16f:         return 8;
			case InternalColorFormat::RGBA32f:         return 16;
			case InternalColorFormat::Depth24:         return 3;
			case InternalColorFormat::Depth24Stencil8: return 4;
			default:
				QK_CORE_FATAL("Invalid internal texture format");
				return 0;
		}
	}

	constexpr bool IsColorFormatAlpha(ColorDataFormat format)
	{
		switch (format)
		{
			case ColorDataFormat::RGBA:
			case ColorDataFormat::BGRA: return true;
			default:                    return false;
		}
	}
}
