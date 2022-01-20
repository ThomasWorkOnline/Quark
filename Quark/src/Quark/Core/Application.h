#pragma once

#include "Core.h"
#include "Layer.h"
#include "Window.h"
#include "AppOptions.h"

#include "../Events/ApplicationEvent.h"
#include "../Renderer/RenderingAPI.h"

namespace Quark {

    class Application
    {
    public:
        Application(const ApplicationOptions& options = ApplicationOptions());
        virtual ~Application();

        void Run();
        void Stop();
        void OnEvent(Event& e);

        void PushLayer(Layer* layer);

        float GetAppRunningTime() const { return m_TotalTime; }
        std::thread::id GetThreadId() const { return m_AppMainThreadId; }

        const Window& GetWindow() const { return *m_Window; }
        Window& GetWindow() { return *m_Window; }

        static Application& Get() { return *s_Instance; }

    private:
        bool OnWindowClosed(WindowClosedEvent& e);
		bool OnWindowResized(WindowResizedEvent& e);

    private:
        static Application* s_Instance;
        std::thread::id m_AppMainThreadId;

        ApplicationOptions m_Options;
        Scope<Window> m_Window;
        std::vector<Layer*> m_Layers;

        float m_TotalTime = 0.0f;
        bool m_Running = true;
    };
}
