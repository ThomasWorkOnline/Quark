#include "qkpch.h"
#include "GraphicsContext.h"
#include "GraphicsAPI.h"

#ifdef QK_PLATFORM_APPLE
#	include "Platform/Metal/MetalGLFWGraphicsContext.h"
#endif

#if defined(QK_PLATFORM_WINDOWS) && defined(QK_USE_NATIVE_APIS)
	// Windows specific graphics context
#	include "Platform/Windows/OpenGL/OpenGLWin32GraphicsContext.h"
#endif

#include "Platform/OpenGL/OpenGLGraphicsContext.h"
#include "Platform/Vulkan/VulkanGraphicsContext.h"

namespace Quark {

	Scope<GraphicsContext> GraphicsContext::Create(void* windowHandle)
	{
		switch (GraphicsAPI::GetAPI())
		{
			case GraphicsAPI::API::Metal:
#ifdef QK_PLATFORM_APPLE
				return CreateScope<MetalContext>(windowHandle);
#else
				QK_CORE_FATAL("Metal is not supported on this platform");
				return nullptr;
#endif

			case GraphicsAPI::API::OpenGL:
#if defined(QK_PLATFORM_WINDOWS) && defined(QK_USE_NATIVE_APIS)
				return CreateScope<OpenGLWin32GraphicsContext>(windowHandle);
#else
				return CreateScope<OpenGLContext>(windowHandle);
#endif

			case GraphicsAPI::API::Vulkan: return CreateScope<VulkanContext>(windowHandle);
			default:
				QK_CORE_FATAL("Unknown graphics API");
				return nullptr;
		}
	}
}
