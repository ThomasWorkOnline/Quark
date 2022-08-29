#include "qkpch.h"

#include "Quark/Renderer/GraphicsAPI.h"

#include "Metal/MetalGraphicsAPI.h"
#include "Platform/OpenGL/OpenGLGraphicsAPI.h"
#include "Platform/Vulkan/VulkanGraphicsAPI.h"

namespace Quark {

	RHI GetDefaultRHIForPlatform()
	{
		return RHI::Metal;
	}

	Scope<GraphicsAPI> CreateRHIForPlatform(RHI api)
	{
		switch (api)
		{
			case RHI::OpenGL: return CreateScope<OpenGLGraphicsAPI>();
			case RHI::Vulkan: return CreateScope<VulkanGraphicsAPI>();
			case RHI::Metal:  return CreateScope<MetalGraphicsAPI>();

			QK_ASSERT_NO_DEFAULT("Graphics API is not supported!");
		}

		return nullptr;
	}
}
