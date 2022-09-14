#pragma once

#include <cstdint>

namespace Quark {

	enum class ColorFormat
	{
		None = 0,

		RGB8,
		RGB16,
		RGB32,

		RGBA8,
		RGBA16,

		// sRGB formats
		RGB8SRGB,
		RGBA8SRGB,

		BGR8SRGB,
		BGRA8SRGB,

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

	enum class SamplerFilterMode
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

	enum class SamplerAddressMode
	{
		None = 0,

		// Tiling formats
		ClampToBorder,
		ClampToEdge,
		Repeat,

		Default = ClampToEdge
	};

	enum class SampleCount
	{
		SampleCount1 = 0,
		SampleCount2,
		SampleCount4,
		SampleCount8,
		SampleCount16,
		SampleCount32,
		SampleCount64
	};

	struct SamplerRenderModes
	{
		SamplerFilterMode MagFilteringMode = SamplerFilterMode::Linear;
		SamplerFilterMode MinFilteringMode = SamplerFilterMode::LinearMipmapLinear;
		SamplerAddressMode AddressMode     = SamplerAddressMode::Default;
	};

	size_t   GetPixelFormatSize(ColorFormat format);
	uint32_t GetMipLevelsForResolution(uint32_t width, uint32_t height);

	bool IsFormatUsingMips(SamplerFilterMode mode);
	bool IsColorFormatAlpha(ColorFormat format);
	bool IsPixel4BytesAligned(ColorFormat format);
}
