#include "qkpch.h"
#include "Formats.h"

namespace Quark {

	static constexpr size_t s_PixelFormatSizeLUT[] = {
		/*None*/            0,

		/*RGB8*/            3 * 1,
		/*RGB16*/           3 * 2,
		/*RGB32UInt*/       3 * 4,

		/*RGBA8*/           4 * 1,
		/*RGBA16*/          4 * 2,

		/*RGB8_SRGB*/       3 * 1,
		/*RGBA8_SRGB*/      4 * 1,

		/*BGR8_SRGB*/       3 * 1,
		/*BGRA8_SRGB*/      4 * 1,

		/*RGB16f*/          3 * 2,
		/*RGB32f*/          3 * 4,
		/*RGBA16f*/         4 * 2,
		/*RGBA32f*/         4 * 4,

		/*Red8*/            1,

		/*Depth24*/         3,
		/*Depth24Stencil8*/ 3 + 1
	};

	static constexpr bool s_PixelFormatAplhaLUT[] = {
		/*None*/            false,

		/*RGB8*/            false,
		/*RGB16*/           false,
		/*RGB32UInt*/       false,

		/*RGBA8*/           true,
		/*RGBA16*/          true,

		/*RGB8_SRGB*/       false,
		/*RGBA8_SRGB*/      true,

		/*BGR8_SRGB*/       false,
		/*BGRA8_SRGB*/      true,

		/*RGB16f*/          false,
		/*RGB32f*/          false,
		/*RGBA16f*/         true,
		/*RGBA32f*/         true,

		/*Red8*/            false,

		/*Depth24*/         false,
		/*Depth24Stencil8*/ false
	};

	static constexpr bool s_FilteringModeHasMipsLUT[] = {
		/*None = 0*/             false,
		/*Nearest*/              false,
		/*Linear*/               false,
		/*NearestMipmapNearest*/ true,
		/*NearestMipmapLinear*/  true,
		/*LinearMipmapNearest*/  true,
		/*LinearMipmapLinear*/   true
	};

	size_t GetPixelFormatSize(ColorFormat format)
	{
		return s_PixelFormatSizeLUT[static_cast<size_t>(format)];
	}

	uint32_t GetMipLevelsForResolution(uint32_t width, uint32_t height)
	{
		uint32_t stride = std::max(width, height);
		uint32_t mips = 0;

		while (stride >>= 1)
			mips++;

		return mips + 1;
	}

	bool IsFormatUsingMips(SamplerFilterMode mode)
	{
		return s_FilteringModeHasMipsLUT[static_cast<size_t>(mode)];
	}

	bool IsColorFormatAlpha(ColorFormat format)
	{
		return s_PixelFormatAplhaLUT[static_cast<size_t>(format)];
	}

	bool IsPixel4BytesAligned(ColorFormat format)
	{
		return GetPixelFormatSize(format) % 4 == 0;
	}
}
