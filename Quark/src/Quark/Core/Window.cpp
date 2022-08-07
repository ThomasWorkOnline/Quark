#include "qkpch.h"
#include "Window.h"

#if defined(QK_PLATFORM_WINDOWS) && defined(QK_USE_NATIVE_APIS)
#	include "Platform/Windows/NativeWindowsWindow.h"
#else
#	include "Platform/GLFW/GLFWWindow.h"
#endif

namespace Quark {

	Scope<Window> Window::Create(const WindowSpecification& spec)
	{
#if defined(QK_PLATFORM_WINDOWS) && defined(QK_USE_NATIVE_APIS)
		return CreateScope<NativeWindowsWindow>(spec);
#else
		return CreateScope<GLFWWindow>(spec);
#endif
	}
}
