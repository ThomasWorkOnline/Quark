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

	Scope<Window> Window::Create(const WindowSpecification& spec, WindowType type)
	{
		switch (type)
		{
			case WindowType::Generic: return CreateScope<GenericWindow>(spec); 
			case WindowType::Native:  return CreateScope<NativeWindow>(spec);

			QK_ASSERT_NO_DEFAULT("Unknown WindowType");
		}

		return nullptr;
	}
}
