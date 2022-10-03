#include "qkpch.h"
#include "ImGuiBackend.h"

#include "Quark/Renderer/Renderer.h"
#include "Quark/Renderer/GraphicsAPI.h"

#include "Platform/OpenGL/OpenGLImGuiBackend.h"
#include "Platform/Vulkan/VulkanImGuiBackend.h"

#ifdef QK_PLATFORM_WINDOWS
#	include "Platform/Windows/OpenGL/OpenGLWin32ImGuiBackend.h"
#endif

#include <imgui.h>
#include <backends/imgui_impl_glfw.cpp>

namespace Quark {

	static Scope<ImGuiBackend> CreateOpenGLBackend(void* windowHandle, bool native)
	{
#ifdef QK_PLATFORM_WINDOWS
		if (native) return CreateScope<OpenGLWin32ImGuiBackend>(windowHandle);
		else        return CreateScope<OpenGLImGuiBackend>(windowHandle);
#endif
		return CreateScope<OpenGLImGuiBackend>(windowHandle);
	}

	static Scope<ImGuiBackend> CreateVulkanBackend(void* windowHandle, bool native)
	{
#ifdef QK_PLATFORM_WINDOWS
		if (native)
		{
			QK_CORE_ASSERT(false, "Native Win32 support for Vulkan is not supported yet!");
			return nullptr;
		}
		else
		{
			return CreateScope<VulkanImGuiBackend>(windowHandle);
		}
#endif
		return CreateScope<VulkanImGuiBackend>(windowHandle);
	}

	Scope<ImGuiBackend> ImGuiBackend::Create(void* windowHandle, bool native)
	{
		switch (GraphicsAPI::GetAPI())
		{
			case RHI::OpenGL: return CreateOpenGLBackend(windowHandle, native);
			case RHI::Vulkan: return CreateVulkanBackend(windowHandle, native);

			QK_ASSERT_NO_DEFAULT("Missing API implementation for ImGui!");
		}

		return nullptr;
	}
}
