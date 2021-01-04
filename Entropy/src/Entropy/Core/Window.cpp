#include "Window.h"

#include "Core.h"

#if defined NT_PLATFORM_WINDOWS
    #include "../../Platform/Windows/WindowsWindow.h"
#elif defined NT_PLATFORM_APPLE
    #include "../../Platform/Macos/MacosWindow.h"
#elif defined NT_PLATFORM_LINUX
    #include "../../Platform/Linux/LinuxWindow.h"
#endif

namespace Entropy {

    Scope<Window> Window::Create(uint32_t width, uint32_t height, const char* title)
    {
        // Platform specefic window creation is done here
#if defined NT_PLATFORM_WINDOWS
        NT_TRACE("Created a new window for Windows");
        return CreateScope<WindowsWindow>(width, height, title);
#elif defined NT_PLATFORM_APPLE
        NT_TRACE("Created a new window for Macos X");
        return CreateScope<MacosWindow>(width, height, title);
#elif defined NT_PLATFORM_LINUX
        NT_TRACE("Created a new window for Linux");
        return CreateScope<LinuxWindow>(width, height, title);
#endif
        NT_FATAL("Could not create window");
        return nullptr;
    }
}
