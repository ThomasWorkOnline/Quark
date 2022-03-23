#include "Window.h"

#include "Core.h"
#include "Platform/GLFW/GLFWWindow.h"

namespace Quark {

	Scope<Window> Window::Create(const WindowSpecification& spec)
	{
		return CreateScope<GLFWWindow>(spec);
	}
}
