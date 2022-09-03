#pragma once

#include "Quark/Core/Window.h"
#include "Quark/Renderer/GraphicsContext.h"

#include <Windows.h>

namespace Quark {

	class NativeWindowsWindow final : public Window
	{
	public:
		NativeWindowsWindow(const WindowSpecification& spec);
		virtual ~NativeWindowsWindow() final override;

		virtual void SetEventCallback(const EventCallbackFn& callback) final override { m_Data.EventCallback = callback; }
		virtual void OnUpdate() final override;

		virtual uint32_t GetWidth() const final override { return m_Data.Width; }
		virtual uint32_t GetHeight() const final override { return m_Data.Height; }
		virtual float GetAspectRatio() const final override { return (float)m_Data.Width / (float)m_Data.Height; }
		virtual std::string_view GetTitle() const final override { return m_Data.Title; }

		virtual Window& SetTitle(std::string title) final override;
		virtual Window& AppendTitle(std::string title) final override;

		virtual void Resize(uint32_t width, uint32_t height) final override;

		virtual void Focus() final override;
		virtual void Minimize() final override;
		virtual void Maximize() final override;
		virtual void Restore() final override;
		virtual void RequestAttention() final override;
		virtual void DisableCursor() final override {}
		virtual void EnableCursor() final override {}
		virtual void SetVSync(bool enabled) final override;
		virtual void SetFullscreen(bool enabled) final override {}

		virtual bool IsFocused() const final override;
		virtual bool IsMinimized() const final override;
		virtual bool IsMaximized() const final override;
		virtual bool IsCursorEnabled() const final override { return false; }
		virtual bool IsVSync() const final override { return false; }
		virtual bool IsFullscreen() const final override { return false; }

		virtual void* GetNativeWindow() const final override { return m_WindowHandle; }

		// Non-Copyable
		NativeWindowsWindow(const NativeWindowsWindow&) = delete;
		NativeWindowsWindow& operator=(const NativeWindowsWindow&) = delete;

		// Non-Movable
		NativeWindowsWindow(NativeWindowsWindow&&) = delete;
		NativeWindowsWindow& operator=(NativeWindowsWindow&&) = delete;

	private:
		void Init();
		void Shutdown();

		static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		static void OnWindowMoved(HWND hWnd, int32_t x, int32_t y);
		static void OnWindowSizeChanged(HWND hWnd, WPARAM wParam, LPARAM lParam);

	private:
		struct WindowData
		{
			std::string Title;
			uint32_t    Width, Height;
			int32_t     Xpos, Ypos;

			bool        Minimized = false;
			bool        Maximized = false;
			double      CursorXpos, CursorYpos;

			Scope<GraphicsContext> Context;
			EventCallbackFn EventCallback;
		};

		WindowData m_Data{};
		HWND m_WindowHandle;
	};
}
