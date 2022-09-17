#include "qkpch.h"
#include "Window.h"

#if defined(QK_PLATFORM_WINDOWS) && QK_USE_NATIVE_APIS
#	include "Platform/Windows/NativeWindowsWindow.h"
#	define NativeWindow NativeWindowsWindow
#else
#	include "NativeWindow.h"
#	define NativeWindow NativeWindow
#endif

namespace Quark {

	Scope<Window> Window::Create(const WindowSpecification& spec)
	{
		return CreateScope<NativeWindow>(spec);
	}
}
