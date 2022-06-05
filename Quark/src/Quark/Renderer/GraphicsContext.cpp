#include "qkpch.h"
#include "GraphicsContext.h"

#include "GraphicsAPI.h"

// Include all supported API's graphics context implementations
#if defined(QK_PLATFORM_APPLE)
	// Apple specific graphics context
#	include "Platform/Metal/MetalGLFWGraphicsContext.h"
#elif defined(QK_PLATFORM_WINDOWS)
	// Windows specific graphics context
#	if defined(QK_USE_NATIVE_APIS)
#		include "Platform/Windows/OpenGL/OpenGLWin32GraphicsContext.h"
#	endif
#endif

#include "Platform/OpenGL/OpenGLGraphicsContext.h"

namespace Quark {

	Scope<GraphicsContext> GraphicsContext::Create(void* window)
	{
		switch (GraphicsAPI::GetAPI())
		{
			case GraphicsAPI::API::Metal:
			{
#ifdef QK_PLATFORM_APPLE
				return CreateScope<MetalGLFWGraphicsContext>(window);
#else
				#error "Metal API is not supported on this platform"
#endif
			}
			case GraphicsAPI::API::OpenGL:
			{
#if defined(QK_PLATFORM_WINDOWS) && defined(QK_USE_NATIVE_APIS)
				return CreateScope<OpenGLWin32GraphicsContext>(window);
#else
				return CreateScope<OpenGLGLFWGraphicsContext>(window);
#endif
			}
			default:
				QK_CORE_FATAL("Graphics API not supported");
				return nullptr;
		}
	}
}
