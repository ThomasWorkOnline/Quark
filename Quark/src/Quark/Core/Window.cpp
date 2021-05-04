#include "Window.h"

#include "Core.h"

#include "../../Platform/GLFW/GenericWindow.h"

namespace Quark {

    Scope<Window> Window::Create(uint32_t width, uint32_t height, const std::string& title)
    {
        return CreateScope<GenericWindow>(width, height, title);
    }
}
