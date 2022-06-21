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

	GraphicsAPI::API GraphicsAPI::GetDefaultForPlatform()
	{
#ifdef QK_PLATFORM_APPLE
		return GraphicsAPI::API::Metal;
#else
		// Default
		return GraphicsAPI::API::OpenGL;
#endif
	}

	Scope<GraphicsAPI> GraphicsAPI::Create(API api)
	{
		s_API = api;
		switch (api)
		{
			case GraphicsAPI::API::Metal:
			{
#ifdef QK_PLATFORM_APPLE
				return CreateScope<MetalGraphicsAPI>();
#else
				return nullptr;
#endif
			}
			case GraphicsAPI::API::OpenGL: return CreateScope<OpenGLGraphicsAPI>();
			case GraphicsAPI::API::Vulkan: return CreateScope<VulkanGraphicsAPI>();
			default:
				QK_CORE_FATAL("Graphics API not supported");
				return nullptr;
		}
	}
}
