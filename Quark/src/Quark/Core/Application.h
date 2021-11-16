#pragma once

#include "Core.h"
#include "Window.h"
#include "../Events/ApplicationEvent.h"
#include "../Renderer/RenderingAPI.h"

namespace Quark {

    enum ApplicationFlags
    {
        ApplicationNoFlags      = 0,
        ShowApiInWindowTitle    = BIT(0)
    };

    struct ApplicationOptions
    {
        uint32_t Width = 1280, Height = 720;
        std::string Title = "Quark Engine";
        ApplicationFlags Flags = ApplicationNoFlags;

        RenderingAPI::API Api = RenderingAPI::API::OpenGL;

        bool HasFlag(ApplicationFlags flag) const
        {
            return Flags & flag;
        }

        ApplicationOptions() = default;
        ApplicationOptions(
            uint32_t width, uint32_t height,
            const std::string& title,
            ApplicationFlags flags = ApplicationNoFlags,
            RenderingAPI::API api = RenderingAPI::API::OpenGL
        )
            : Width(width), Height(height),
            Title(title), Flags(flags), Api(api)
        {
        }
    };

    class Application
    {
    public:
        Application(const ApplicationOptions& options = ApplicationOptions());
        virtual ~Application();

        virtual void OnUpdate(float deltaTime) {}
        virtual void OnEvent(Event& e) {}

        const Window& GetWindow() const { return *m_Window; }
        Window& GetWindow() { return *m_Window; }

        static Application& Get() { return *s_Instance; }
        std::thread::id GetThreadId() const { return m_AppMainThreadId; }

        void Run();
        void Stop();

    private:
        void OnEventInternal(Event& e);
        
        bool OnWindowClosed(WindowClosedEvent& e);
		bool OnWindowResized(WindowResizedEvent& e);

    private:
        static Application* s_Instance;

        ApplicationOptions m_Options;
        Scope<Window> m_Window;

        std::thread::id m_AppMainThreadId;
        bool m_Running = true;
    };
}
