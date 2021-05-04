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

	enum class TextureFilteringFormat
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

	enum class TextureTilingFormat
	{
		None = 0,

		// Tiling formats
		ChampToBorder,
		ClampToEdge,
		Repeat,

		Default = ChampToBorder
	};
}
