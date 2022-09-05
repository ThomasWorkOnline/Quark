#pragma once

#include "Quark/Core/Core.h"
#include "Quark/Core/Input.h"

#include "Quark/Event/ApplicationEvent.h"
#include "Quark/Event/KeyEvent.h"
#include "Quark/Event/MouseEvent.h"
#include "Quark/Event/WindowEvent.h"

namespace Quark {

	struct WindowSpecification
	{
		std::string Title;
		uint32_t    Width, Height;
		uint32_t    Samples = 1;
	};

	class Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		Window() = default;
		virtual ~Window() = default;

		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual void OnUpdate() = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual float GetAspectRatio() const = 0;
		virtual std::string_view GetTitle() const = 0;

		virtual Window& SetTitle(std::string title) = 0;
		virtual Window& AppendTitle(std::string title) = 0;

		virtual void Resize(uint32_t width, uint32_t height) = 0;

		virtual void Focus() = 0;
		virtual void Minimize() = 0;
		virtual void Maximize() = 0;
		virtual void Restore() = 0;
		virtual void RequestAttention() = 0;
		virtual void DisableCursor() = 0;
		virtual void EnableCursor() = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual void SetFullscreen(bool enabled) = 0;

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
