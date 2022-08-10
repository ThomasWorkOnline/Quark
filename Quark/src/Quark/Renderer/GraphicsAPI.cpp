#include "qkpch.h"
#include "GraphicsAPI.h"

// Include all supported APIs here
#ifdef QK_PLATFORM_APPLE
#	include "Platform/MacOS/Metal/MetalGraphicsAPI.h"
#endif

#include "Platform/OpenGL/OpenGLGraphicsAPI.h"
#include "Platform/Vulkan/VulkanGraphicsAPI.h"

namespace Quark {

	RHI GraphicsAPI::s_API = RHI::None;
	Scope<GraphicsAPI> GraphicsAPI::Instance = Instantiate(GetDefaultForPlatform());

	Scope<GraphicsAPI> GraphicsAPI::Instantiate(RHI api)
	{
		s_API = api;
		switch (api)
		{
#ifdef QK_PLATFORM_APPLE
			case RHI::Metal:  return CreateScope<MetalGraphicsAPI>();
#endif
			case RHI::OpenGL: return CreateScope<OpenGLGraphicsAPI>();
			case RHI::Vulkan: return CreateScope<VulkanGraphicsAPI>();

			QK_ASSERT_NO_DEFAULT("Unknown graphics API");
		}
		
		return nullptr;
	}
}
