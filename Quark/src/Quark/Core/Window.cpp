#include "Window.h"

#include "Core.h"

#include "../../Platform/GLFW/GenericWindow.h"

namespace Quark {

    Scope<Window> Window::Create(const WindowSpecification& spec)
    {
        return CreateScope<GenericWindow>(spec);
    }
}
