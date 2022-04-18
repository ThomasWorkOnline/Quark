#pragma once

namespace Quark {

	enum class TextureDataFormat
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

	enum class TextureInternalFormat
	{
		None = 0,

		RGB8,
		RGB10,
		RGB12,
		RGB16,

		RGBA8,
		RGBA12,
		RGBA16,

		// sRGB formats
		SRGB8,
		SRGBA8,

		// HDR formats
		RGB16f,
		RGB32f,

		Red8,

		// Depth formats
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
		TextureTilingMode    TilingMode       = TextureTilingMode::Default;
	};

	constexpr bool IsformatUsingMips(TextureFilteringMode mode)
	{
		switch (mode)
		{
			case TextureFilteringMode::LinearMipmapLinear:
			case TextureFilteringMode::LinearMipmapNearest:
			case TextureFilteringMode::NearestMipmapLinear:
			case TextureFilteringMode::NearestMipmapNearest:
				return true;
			default:
				return false;
		}
	}
}
