#include "qkpch.h"
#include "GraphicsContext.h"

#include "RenderingAPI.h"

// Include all supported API's graphics context implementations
#include "Platform/OpenGL/OpenGLGraphicsContext.h"

namespace Quark {

	Scope<GraphicsContext> GraphicsContext::Create(void* window)
	{
		switch (RenderingAPI::GetAPI())
		{
			case RenderingAPI::API::OpenGL:
				return CreateScope<OpenGLGraphicsContext>(window);
			default:
				QK_CORE_FATAL("Rendering API not supported");
				return nullptr;
		}
	}
}
