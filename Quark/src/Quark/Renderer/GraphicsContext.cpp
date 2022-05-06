#include "qkpch.h"
#include "GraphicsContext.h"

#include "RenderingAPI.h"

// Include all supported API's graphics context implementations

#if defined(QK_PLATFORM_WINDOWS) && QK_USE_NATIVE_APIS
#	include "Platform/Windows/OpenGL/OpenGLWin32GraphicsContext.h"
#else
#	include "Platform/Standalone/OpenGL/OpenGLGraphicsContext.h"
#endif

namespace Quark {

	Scope<GraphicsContext> GraphicsContext::Create(void* window)
	{
		switch (RenderingAPI::GetAPI())
		{
			case RenderingAPI::API::OpenGL:
			{
#if defined(QK_PLATFORM_WINDOWS) && QK_USE_NATIVE_APIS
				return CreateScope<OpenGLWin32GraphicsContext>(window);
#else
				return CreateScope<OpenGLGLFWGraphicsContext>(window);
#endif
			}
			default:
				QK_CORE_FATAL("Rendering API not supported");
				return nullptr;
		}
	}
}
