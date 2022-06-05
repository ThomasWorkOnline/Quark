#pragma once

#include "Quark/Core/Window.h"
#include "Quark/Renderer/GraphicsContext.h"

typedef struct HWND__* HWND;

namespace Quark {

	class NativeWindowsWindow final : public Window
	{
	public:
		NativeWindowsWindow(const WindowSpecification& spec);
		virtual ~NativeWindowsWindow() override;

		virtual void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
		virtual void OnUpdate() override;

		virtual uint32_t GetWidth() const override { return m_Data.Width; }
		virtual uint32_t GetHeight() const override { return m_Data.Height; }
		virtual std::string_view GetTitle() const override { return m_Data.Title; }

		virtual void SetTitle(const std::string& title) override;
		virtual void AppendTitle(const std::string& title) override;

		virtual void Focus() override;
		virtual void Minimize() override;
		virtual void Maximize() override;
		virtual void Restore() override;
		virtual void RequestAttention() override;
		virtual void DisableCursor() override {}
		virtual void EnableCursor() override {}
		virtual void SetVSync(bool enabled) override;
		virtual void SetFullScreen(bool enabled) override {}

		virtual bool IsFocused() const override;
		virtual bool IsMinimized() const override;
		virtual bool IsMaximized() const override;
		virtual bool IsCursorEnabled() const override { return false; }
		virtual bool IsVSync() const override { return false; }
		virtual bool IsFullscreen() const override { return false; }

		virtual void* GetNativeWindow() const override { return m_WindowHandle; }

		struct WindowData
		{
			std::string Title;
			uint32_t    Width, Height;
			int32_t     Xpos, Ypos;

			bool        Minimized = false;
			bool        Maximized = false;
			double      CursorXpos, CursorYpos;

			EventCallbackFn EventCallback;
		};

	private:
		void Init();
		void Shutdown();

	private:
		Scope<GraphicsContext> m_Context;
		WindowData m_Data{};

		HWND m_WindowHandle;
	};
}