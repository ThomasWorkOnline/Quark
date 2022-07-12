#include "qkpch.h"
#include "GraphicsContext.h"
#include "GraphicsAPI.h"

#ifdef QK_PLATFORM_APPLE
#	include "Platform/Macos/Metal/MetalContext.h"
#endif

#if defined(QK_PLATFORM_WINDOWS) && defined(QK_USE_NATIVE_APIS)
	// Windows specific graphics context
#	include "Platform/Windows/OpenGL/OpenGLWin32Context.h"
#endif

#include "Platform/OpenGL/OpenGLContext.h"
#include "Platform/Vulkan/VulkanContext.h"

namespace Quark {

	Scope<GraphicsContext> GraphicsContext::Create(void* windowHandle)
	{
		switch (GraphicsAPI::GetAPI())
		{
			case API::Metal:
#ifdef QK_PLATFORM_APPLE
				return CreateScope<MetalContext>(windowHandle);
#else
				QK_CORE_FATAL("Metal is not supported on this platform");
				return nullptr;
#endif

			case API::OpenGL:
#if defined(QK_PLATFORM_WINDOWS) && defined(QK_USE_NATIVE_APIS)
				return CreateScope<OpenGLWin32Context>(windowHandle);
#else
				return CreateScope<OpenGLContext>(windowHandle);
#endif

			case API::Vulkan: return CreateScope<VulkanContext>(windowHandle);
			default:
				QK_CORE_FATAL("Unknown graphics API");
				return nullptr;
		}
	}
}
