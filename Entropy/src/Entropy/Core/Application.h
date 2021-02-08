#pragma once

#include "Window.h"
#include "../Events/ApplicationEvent.h"

namespace Entropy {

    class Application
    {
    public:
        Application(uint32_t width = 1280, uint32_t height = 720, const char* title = "Entropy Engine");
        virtual ~Application();

        virtual void OnUpdate(float deltaTime) {}
        virtual void OnCreate() {}
        virtual void OnApplicationEvent(Event& e) {}

        inline Window& GetWindow() { return *m_Window; }

        inline static Application& Get() { return *s_Instance; };

        void Stop();
        void Run();

    private:
        void OnEvent(Event& e);
        
        bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

        static Application* s_Instance;
        Scope<Window> m_Window;
        bool m_Running = true;
    };
}
