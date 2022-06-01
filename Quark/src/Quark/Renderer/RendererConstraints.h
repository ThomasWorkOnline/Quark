#pragma once

#include "Quark/Core/Core.h"

namespace Quark {

	struct FramebufferHardwareConstraints
	{
		uint32_t MaxWidth;
		uint32_t MaxHeight;
	};

	struct UniformBufferHardwareConstraints
	{
		size_t MaxBufferSize;
		uint32_t MaxBindings;
	};

	struct TextureHardwareConstraints
	{
		uint32_t MaxTextureSlots;
		uint32_t MaxPixelSize;
		uint32_t MaxTextureArrayLayers;
	};

	struct HardwareConstraints
	{
		FramebufferHardwareConstraints   FramebufferConstraints;
		UniformBufferHardwareConstraints UniformBufferConstraints;
		TextureHardwareConstraints       TextureConstraints;
	};
}
