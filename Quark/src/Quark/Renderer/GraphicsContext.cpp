#include "qkpch.h"
#include "GraphicsContext.h"
#include "GraphicsAPI.h"

#ifdef QK_PLATFORM_WINDOWS
#	include "Platform/Direct3D/Direct3DContext.h"
#endif

#ifdef QK_PLATFORM_APPLE
#	include "Platform/Apple/Metal/MetalContext.h"
#endif

#include "Platform/OpenGL/OpenGLContext.h"
#include "Platform/Vulkan/VulkanContext.h"

#if defined(QK_PLATFORM_WINDOWS)
	// Windows specific graphics context
#	include "Platform/Windows/OpenGL/OpenGLWin32Context.h"
#endif

namespace Quark {

	Scope<GraphicsContext> CreateOpenGLContext(void* windowHandle, bool native)
	{
#if defined(QK_PLATFORM_WINDOWS)
		if (native) return CreateScope<OpenGLWin32Context>(windowHandle);
		else        return CreateScope<OpenGLContext>(windowHandle);
#endif
		(void)native;
		return CreateScope<OpenGLContext>(windowHandle);
	}

	Scope<GraphicsContext> CreateVulkanContext(void* windowHandle, bool native)
	{
#if defined(QK_PLATFORM_WINDOWS)
		if (native)
		{
			QK_CORE_ASSERT(false, "Native Win32 support for Vulkan is not supported yet!");
			return nullptr;
		}
		else
		{
			return CreateScope<VulkanContext>(windowHandle);
		}
#endif
		(void)native;
		return CreateScope<VulkanContext>(windowHandle);
	}

#if defined(QK_PLATFORM_WINDOWS)
	Scope<GraphicsContext> CreateDirect3D12Context(void* windowHandle, bool native)
	{
		if (native)
		{
			QK_CORE_ASSERT(false, "Native Win32 support for Direct3D12 is not supported yet!");
			return nullptr;
		}
		else
		{
			return CreateScope<Direct3D12Context>(windowHandle);
		}
	}
#endif

	Scope<GraphicsContext> GraphicsContext::Create(void* windowHandle, bool native)
	{
		switch (GraphicsAPI::GetAPI())
		{
#ifdef QK_PLATFORM_WINDOWS
			case RHI::Direct3D12: return CreateDirect3D12Context(windowHandle, native);
#endif

#ifdef QK_PLATFORM_APPLE
			case RHI::Metal:      return CreateScope<MetalContext>(windowHandle);
#endif
			case RHI::OpenGL:     return CreateOpenGLContext(windowHandle, native);
			case RHI::Vulkan:     return CreateVulkanContext(windowHandle, native);

			QK_ASSERT_NO_DEFAULT("Unknown graphics API");
		}

		return nullptr;
	}
}
