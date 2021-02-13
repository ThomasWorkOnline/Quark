#include "Window.h"

#include "Core.h"

#include "../../Platform/GLFW/GenericWindow.h"

namespace Entropy {

    Scope<Window> Window::Create(uint32_t width, uint32_t height, const char* title)
    {
        return CreateScope<GenericWindow>(width, height, title);
        NT_FATAL("Could not create window");
        return nullptr;
    }
}
