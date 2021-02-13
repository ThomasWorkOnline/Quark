#pragma once

namespace Entropy {

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

		Default = Linear
	};

	enum class TextureTilingFormat
	{
		None = 0,

		// Tiling formats
		Clamp,
		Repeat,

		Default = Clamp
	};
}
