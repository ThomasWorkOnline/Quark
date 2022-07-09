#include "qkpch.h"
#include "GraphicsAPI.h"

// Include all supported APIs here
#ifdef QK_PLATFORM_APPLE
#	include "Platform/Metal/MetalGraphicsAPI.h"
#endif

#include "Platform/OpenGL/OpenGLGraphicsAPI.h"
#include "Platform/Vulkan/VulkanGraphicsAPI.h"

#define USE_VULKAN 0

namespace Quark {

	API GraphicsAPI::s_API = API::None;
	Scope<GraphicsAPI> GraphicsAPI::Instance = Instantiate(GetDefaultForPlatform());

	API GraphicsAPI::GetDefaultForPlatform()
	{
#ifdef QK_PLATFORM_APPLE
		return API::Metal;
#else
		// Default
#if USE_VULKAN
		return API::Vulkan;
#else
		return API::OpenGL;
#endif
#endif
	}

	Scope<GraphicsAPI> GraphicsAPI::Instantiate(API api)
	{
		s_API = api;
		switch (api)
		{
			case API::Metal:
			{
#ifdef QK_PLATFORM_APPLE
				return CreateScope<MetalGraphicsAPI>();
#else
				QK_CORE_FATAL("Metal is not supported on this platform");
				return nullptr;
#endif
			}
			case API::OpenGL: return CreateScope<OpenGLGraphicsAPI>();
			case API::Vulkan: return CreateScope<VulkanGraphicsAPI>();
			default:
				QK_CORE_FATAL("Unknown graphics API");
				return nullptr;
		}
	}
}
