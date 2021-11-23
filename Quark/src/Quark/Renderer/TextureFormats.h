#pragma once

namespace Quark {

	enum class TextureDataFormat
	{
		None = 0,

		// Color formats
		RGB8,
		RGBA8,

		// Depth formats
		Depth24Stencil8,

		Default = RGBA8
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
		TextureFilteringMode MagFilteringMode = TextureFilteringMode::Linear;
		TextureFilteringMode MinFilteringMode = TextureFilteringMode::LinearMipmapLinear;
		TextureTilingMode TilingMode = TextureTilingMode::Default;
	};

	bool IsformatUsingMips(TextureFilteringMode mode);
}
