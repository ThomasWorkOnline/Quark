#pragma once

#include "Window.h"
#include "Core.h"
#include "../Events/ApplicationEvent.h"

extern int main();

namespace Quark {

    enum class ApplicationFlag
    {
        None = 0,
        ShowAPI = BIT(0)
    };

    class Application
    {
    public:
        Application(uint32_t width = 1280, uint32_t height = 720, const std::string& title = "Quark Engine", ApplicationFlag flags = ApplicationFlag::None);
        virtual ~Application();

        virtual void OnCreate() {}
        virtual void OnUpdate(float deltaTime) {}
        virtual void OnDestroy() {}
        virtual void OnEvent(Event& e) {}

        bool HasFlag(ApplicationFlag flag) const { return (uint32_t)m_Flags & (uint32_t)flag; }

        const Window& GetWindow() const { return *m_Window; }
        Window& GetWindow() { return *m_Window; }

        const std::atomic<bool>& IsRunning() const { return m_Running; };

        static Application& Get() { return *s_Instance; };

        std::thread::id GetThreadID() const { return s_AppMainThreadId; }

        void Stop();

    private:
        void Run();

        void OnEventInternal(Event& e);
        
        bool OnWindowClose(WindowClosedEvent& e);
		bool OnWindowResize(WindowResizedEvent& e);

        friend int ::main();

    private:
        static Application* s_Instance;
        static std::thread::id s_AppMainThreadId;
        Scope<Window> m_Window;

        ApplicationFlag m_Flags;

        std::atomic<bool> m_Running = true;
    };
}
