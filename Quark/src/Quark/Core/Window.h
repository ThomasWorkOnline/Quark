#pragma once

#include "../Core/Core.h"
#include "../Core/Input.h"

#include "../Events/ApplicationEvent.h"
#include "../Events/KeyEvent.h"
#include "../Events/MouseEvent.h"

namespace Quark {

    struct WindowSpecification
    {
        uint32_t Width, Height;
        std::string Title;
        uint32_t Samples = 1;
    };

    class Window
    {
    public:
        using EventCallbackFn = std::function<void(Event&)>;

        virtual ~Window() = default;

        virtual void OnUpdate() = 0;

        virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

        virtual const std::string& GetTitle() const = 0;
        virtual void SetTitle(const std::string& title) = 0;
        virtual void AppendTitle(const std::string& title) = 0;

        virtual void SetEventCallback(const EventCallbackFn& callback) = 0;

        virtual void Select() = 0;
        virtual void Deselect() = 0;
        virtual bool IsSelected() const = 0;
        virtual bool IsCursorDisabled() const = 0;
        virtual bool IsCursorNormal() const = 0;
        virtual void SetVSync(bool enabled) = 0;
        virtual bool IsVSync() const = 0;
        virtual void SetFullScreen(bool enabled) = 0;
        virtual bool IsFullscreen() const = 0;

        virtual void* GetNativeWindow() const = 0;

        static Scope<Window> Create(const WindowSpecification& spec);
    };
}
