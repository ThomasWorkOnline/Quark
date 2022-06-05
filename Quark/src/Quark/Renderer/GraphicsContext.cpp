#include "qkpch.h"
#include "GraphicsContext.h"

#include "GraphicsAPI.h"

// Include all supported API's graphics context implementations

#if defined(QK_PLATFORM_WINDOWS) && defined(QK_USE_NATIVE_APIS)
#	include "Platform/Windows/OpenGL/OpenGLWin32GraphicsContext.h"
#else
#	include "Platform/GLFW/OpenGL/OpenGLGraphicsContext.h"
#endif

namespace Quark {

	Scope<GraphicsContext> GraphicsContext::Create(void* window)
	{
		switch (GraphicsAPI::GetAPI())
		{
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
