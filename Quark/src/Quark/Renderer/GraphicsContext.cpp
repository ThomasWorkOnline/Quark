#include "qkpch.h"
#include "GraphicsContext.h"
#include "GraphicsAPI.h"

#ifdef QK_PLATFORM_WINDOWS
#	include "Platform/Direct3D/Direct3DContext.h"
#endif

#ifdef QK_PLATFORM_APPLE
#	include "Platform/MacOS/Metal/MetalContext.h"
#endif

#include "Platform/OpenGL/OpenGLContext.h"
#include "Platform/Vulkan/VulkanContext.h"

#if defined(QK_PLATFORM_WINDOWS) && QK_USE_NATIVE_APIS
	// Windows specific graphics context
#	include "Platform/Windows/OpenGL/OpenGLWin32Context.h"
#	define OpenGLContext OpenGLWin32Context
#else
#	define OpenGLContext OpenGLContext
#endif

namespace Quark {

	Scope<GraphicsContext> GraphicsContext::Create(void* windowHandle)
	{
		switch (GraphicsAPI::GetAPI())
		{
			case RHI::Direct3D12: return CreateScope<Direct3D12Context>(windowHandle);

#ifdef QK_PLATFORM_APPLE
			case RHI::Metal: return CreateScope<MetalContext>(windowHandle);
#endif

			case RHI::OpenGL: return CreateScope<OpenGLContext>(windowHandle);
			case RHI::Vulkan: return CreateScope<VulkanContext>(windowHandle);

			QK_ASSERT_NO_DEFAULT("Unknown graphics API");
		}

		return nullptr;
	}
}
