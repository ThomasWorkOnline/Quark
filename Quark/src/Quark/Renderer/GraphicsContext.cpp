#include "qkpch.h"
#include "GraphicsContext.h"
#include "GraphicsAPI.h"

#ifdef QK_PLATFORM_APPLE
#	include "Platform/MacOS/Metal/MetalContext.h"
#endif

#if defined(QK_PLATFORM_WINDOWS) && QK_USE_NATIVE_APIS
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
#ifdef QK_PLATFORM_APPLE
			case RHI::Metal: return CreateScope<MetalContext>(windowHandle);
#endif

			case RHI::OpenGL:
#if defined(QK_PLATFORM_WINDOWS) && QK_USE_NATIVE_APIS
				return CreateScope<OpenGLWin32Context>(windowHandle);
#else
				return CreateScope<OpenGLContext>(windowHandle);
#endif

			case RHI::Vulkan: return CreateScope<VulkanContext>(windowHandle);

			QK_ASSERT_NO_DEFAULT("Unknown graphics API");
		}

		return nullptr;
	}
}
