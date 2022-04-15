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

		Default = RGBA
	};

	enum class TextureInternalFormat
	{
		None = 0,

		RGB8,
		RGBA8,

		// sRGB formats
		SRGB8,
		SRGBA8,

		Red8,

		// Depth formats
		Depth24Stencil8,

		Default = SRGBA8
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
		LinearMipmapLinear,

		Default = Linear
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
		TextureFilteringMode MagFilteringMode = TextureFilteringMode::Default;
		TextureFilteringMode MinFilteringMode = TextureFilteringMode::Default;
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
