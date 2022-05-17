#pragma once

#include "Quark/Core/Core.h"
#include "Quark/Core/Input.h"
#include "Quark/Events/ApplicationEvent.h"
#include "Quark/Events/KeyEvent.h"
#include "Quark/Events/MouseEvent.h"
#include "Quark/Events/WindowEvents.h"

namespace Quark {

	struct WindowSpecification
	{
		uint32_t    Width, Height;
		std::string Title;
		uint32_t    Samples = 1;
	};

	class Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		Window() = default;
		virtual ~Window() = default;

		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;

		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual void OnUpdate() = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual const std::string& GetTitle() const = 0;

		virtual void SetTitle(const std::string& title) = 0;
		virtual void AppendTitle(const std::string& title) = 0;

		virtual void Focus() = 0;
		virtual void Minimize() = 0;
		virtual void Maximize() = 0;
		virtual void Restore() = 0;
		virtual void RequestAttention() = 0;
		virtual void DisableCursor() = 0;
		virtual void EnableCursor() = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual void SetFullScreen(bool enabled) = 0;

		virtual bool IsFocused() const = 0;
		virtual bool IsMinimized() const = 0;
		virtual bool IsMaximized() const = 0;
		virtual bool IsCursorEnabled() const = 0;
		virtual bool IsVSync() const = 0;
		virtual bool IsFullscreen() const = 0;

		virtual void* GetNativeWindow() const = 0;

		static Scope<Window> Create(const WindowSpecification& spec);
	};
}
