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

	struct UniformBufferCapabilities
	{
		size_t MaxBufferSize;
		uint32_t MaxBindings;
	};

	struct TextureCapabilities
	{
		uint32_t MaxTextureSlots;
		uint32_t MaxPixelSize;
		uint32_t MaxTextureArrayLayers;
	};

	struct GraphicsAPICapabilities
	{
		FramebufferCapabilities   FramebufferCapabilities;
		TextureCapabilities       TextureCapabilities;
		UniformBufferCapabilities UniformBufferCapabilities;
	};
}
