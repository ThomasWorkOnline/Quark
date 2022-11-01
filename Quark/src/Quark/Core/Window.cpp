#include "qkpch.h"
#include "Window.h"

#if defined(QK_PLATFORM_WINDOWS)
#	include "Platform/Windows/NativeWindowsWindow.h"
#endif

#include "GenericWindow.h"

namespace Quark {

#if defined(QK_PLATFORM_WINDOWS)
	using NativeWindow = NativeWindowsWindow;
#else
	using NativeWindow = GenericWindow;
#endif

	Scope<Window> Window::Create(const WindowSpecification& spec, bool native)
	{
		if (native)
		{
			return CreateScope<NativeWindow>(spec);
		}
		else
		{
			return CreateScope<GenericWindow>(spec);
		}
	}
}
