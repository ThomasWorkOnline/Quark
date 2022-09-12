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

	enum class TextureFilteringMode
	{
		None = 0,

		// Filter formats
		Nearest,
		Linear,
		NearestMipmapNearest,
		NearestMipmapLinear,
		LinearMipmapNearest,
		LinearMipmapLinear
	};

	enum class TextureTilingMode
	{
		None = 0,

		// Tiling formats
		ClampToBorder,
		ClampToEdge,
		Repeat,

		Default = ClampToEdge
	};

	struct TextureRenderModes
	{
		TextureFilteringMode MagFilteringMode = TextureFilteringMode::Linear;
		TextureFilteringMode MinFilteringMode = TextureFilteringMode::LinearMipmapLinear;
		TextureTilingMode    TilingMode = TextureTilingMode::Default;
	};

	size_t   GetPixelFormatSize(ColorDataFormat format);
	uint32_t GetMipLevelsForResolution(uint32_t width, uint32_t height);

	bool IsFormatUsingMips(TextureFilteringMode mode);
	bool IsColorFormatAlpha(ColorDataFormat format);
	bool IsPixel4BytesAligned(ColorDataFormat format);
}
