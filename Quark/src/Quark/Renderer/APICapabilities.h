#pragma once

#include "Quark/Core/Core.h"

namespace Quark {

	// Supported APIs
	enum class RHI
	{
		None = 0,
		OpenGL,
		Vulkan,
		Metal
	};

	struct FramebufferCapabilities
	{
		uint32_t MaxWidth;
		uint32_t MaxHeight;
		uint32_t MaxAttachments;
	};

	struct SamplerCapabilities
	{
		uint32_t MaxPerStageSamplers;
	};

	struct UniformBufferCapabilities
	{
		size_t   MaxBufferSize;
		uint32_t MaxPerStageBuffers;
	};

	struct TextureCapabilities
	{
		uint32_t MaxWidth;
		uint32_t MaxHeight;
		uint32_t MaxDepth;
		uint32_t MaxArrayLayers;
	};

	struct GraphicsAPICapabilities
	{
		FramebufferCapabilities   FramebufferCapabilities;
		SamplerCapabilities       SamplerCapabilities;
		TextureCapabilities       TextureCapabilities;
		UniformBufferCapabilities UniformBufferCapabilities;
	};
}
