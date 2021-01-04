#pragma once

#include "Window.h"
#include "../Events/ApplicationEvent.h"

int main();

namespace Entropy {

    // Application class containing data
    class Application
    {
    public:
        Application(uint32_t width = 1280, uint32_t height = 720, const char* title = "Entropy Engine");
        virtual ~Application();

        virtual void OnUpdate(float deltaTime) {}
        virtual void OnCreate() {}
        virtual void OnApplicationEvent(Event& e) {}

        void OnEvent(Event& e);

        inline Window& GetWindow() { return *m_Window; }

        inline static Application& Get() { return *s_Instance; };

        void Stop();

    private:
        void Run();
        
        bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

        friend int ::main();

        static Application* s_Instance;
        Scope<Window> m_Window;
        bool m_Running = true;
    };

    Application* CreateApplication();
}
