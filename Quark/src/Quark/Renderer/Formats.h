#pragma once

#include <cstdint>

namespace Quark {

	enum class ColorFormat
	{
		None = 0,

		RGB8,
		RGB16,
		RGB32UInt,

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
		Depth16,
		Depth24,
		Depth24Stencil8,

		Depth32f
	};

	enum class SamplerFilterMode
	{
		Nearest = 0,
		Linear
	};

	enum class SamplerMipmapMode
	{
		Nearest = 0,
		Linear
	};

	enum class SamplerAddressMode
	{
		// Tiling formats
		ClampToEdge = 0,
		ClampToBorder,
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
		SamplerFilterMode  MagFilteringMode{};
		SamplerFilterMode  MinFilteringMode{};
		SamplerMipmapMode  MipmapMode{};
		SamplerAddressMode AddressMode{};
	};

	size_t GetPixelFormatSize(ColorFormat format);
	SampleCount GetHighestSampleCount(uint32_t samples);

	uint32_t GetMipLevelsForResolution(uint32_t width, uint32_t height);
	uint32_t GetIntegerSampleCount(SampleCount samples);

	bool IsFormatUsingMips(SamplerFilterMode mode);
	bool IsColorFormatAlpha(ColorFormat format);
	bool IsPixel4BytesAligned(ColorFormat format);
}
