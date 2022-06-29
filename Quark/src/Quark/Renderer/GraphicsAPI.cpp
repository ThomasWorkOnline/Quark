#include "qkpch.h"
#include "GraphicsAPI.h"

// Include all supported APIs here
#ifdef QK_PLATFORM_APPLE
#	include "Platform/Metal/MetalGraphicsAPI.h"
#endif

#include "Platform/OpenGL/OpenGLGraphicsAPI.h"
#include "Platform/Vulkan/VulkanGraphicsAPI.h"

namespace Quark {

	GraphicsAPI::API GraphicsAPI::s_API = GraphicsAPI::API::None;
	Scope<GraphicsAPI> GraphicsAPI::Instance = Instantiate(GetDefaultForPlatform());

	GraphicsAPI::API GraphicsAPI::GetDefaultForPlatform()
	{
#ifdef QK_PLATFORM_APPLE
		return GraphicsAPI::API::Metal;
#else
		// Default
		return GraphicsAPI::API::OpenGL;
		//return GraphicsAPI::API::Vulkan;
#endif
	}

	Scope<GraphicsAPI> GraphicsAPI::Instantiate(API api)
	{
		s_API = api;
		switch (api)
		{
			case GraphicsAPI::API::Metal:
			{
#ifdef QK_PLATFORM_APPLE
				return CreateScope<MetalGraphicsAPI>();
#else
				QK_CORE_FATAL("Metal is not supported on this platform");
				return nullptr;
#endif
			}
			case GraphicsAPI::API::OpenGL: return CreateScope<OpenGLGraphicsAPI>();
			case GraphicsAPI::API::Vulkan: return CreateScope<VulkanGraphicsAPI>();
			default:
				QK_CORE_FATAL("Unknown graphics API");
				return nullptr;
		}
	}
}
