#pragma once

namespace Quark {

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
			case TextureFilteringMode::Nearest:
			case TextureFilteringMode::Linear:               return false;
			case TextureFilteringMode::NearestMipmapNearest:
			case TextureFilteringMode::NearestMipmapLinear:
			case TextureFilteringMode::LinearMipmapNearest:
			case TextureFilteringMode::LinearMipmapLinear:   return true;

			QK_ASSERT_NO_DEFAULT("Invalid texture filtering mode");
		}

		return false;
	}

	inline uint32_t GetMipLevelsForResolution(uint32_t width, uint32_t height)
	{
		uint32_t stride = std::max(width, height);
		uint32_t mips = 0;

		while (stride >>= 1)
			mips++;

		return mips + 1;
	}
}
