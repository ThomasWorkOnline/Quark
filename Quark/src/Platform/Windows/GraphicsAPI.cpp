#include "qkpch.h"
#include "GraphicsAPI.h"

#include "Platform/OpenGL/OpenGLGraphicsAPI.h"
#include "Platform/Vulkan/VulkanGraphicsAPI.h"

#define USE_VULKAN 1

namespace Quark {

	RHI GetDefaultRHIForPlatform()
	{
#if USE_VULKAN
		return RHI::Vulkan;
#else
		return RHI::OpenGL;
#endif
	}

	Scope<GraphicsAPI> CreateDefaultRHIForPlatform()
	{
#if USE_VULKAN
		return CreateScope<VulkanGraphicsAPI>();
#else
		return CreateScope<OpenGLGraphicsAPI>();
#endif
	}
}
