#pragma once

#include "Window.h"
#include "../Events/ApplicationEvent.h"

extern int main();

namespace Entropy {

    class Application
    {
    public:
        Application(uint32_t width = 1280, uint32_t height = 720, const std::string& title = "Entropy Engine");
        virtual ~Application();

        virtual void OnCreate() {}
        virtual void OnUpdate(float deltaTime) {}
        virtual void OnDestroy() {}
        virtual void OnEvent(Event& e) {}

        inline Window& GetWindow() { return *m_Window; }

        inline static Application& Get() { return *s_Instance; };

        void Stop();

    private:
        void Run();

        void OnEventInternal(Event& e);
        
        bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

        friend int ::main();

        static Application* s_Instance;
        Scope<Window> m_Window;
        bool m_Running = true;
    };
}
