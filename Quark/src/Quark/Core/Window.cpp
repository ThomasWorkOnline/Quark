#include "qkpch.h"
#include "Window.h"

#include "Core.h"

#if defined(QK_PLATFORM_WINDOWS) && QK_USE_NATIVE_APIS
#	include "Platform/Windows/WindowsWindow.h"
#else
#	include "Platform/GLFW/GLFWWindow.h"
#endif

namespace Quark {

	Scope<Window> Window::Create(const WindowSpecification& spec)
	{
#if defined(QK_PLATFORM_WINDOWS) && QK_USE_NATIVE_APIS
		return CreateScope<WindowsWindow>(spec);
#else
		return CreateScope<GLFWWindow>(spec);
#endif
	}
}
