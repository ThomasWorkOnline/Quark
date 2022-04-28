#pragma once

#include "Quark/Core/Window.h"
#include "Quark/Renderer/GraphicsContext.h"

typedef HWND;

namespace Quark {

	class WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowSpecification& spec);
		~WindowsWindow() override;

		void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
		void OnUpdate() override;

		uint32_t GetWidth() const override { return 0; }
		uint32_t GetHeight() const override { return 0; }
		const std::string& GetTitle() const override { return m_Data.Title; }

		void SetTitle(const std::string& title) override {}
		void AppendTitle(const std::string& title) override {}

		void Focus() override {}
		void Minimize() override {}
		void Maximize() override {}
		void Restore() override {}
		void RequestAttention() override {}
		void DisableCursor() override {}
		void EnableCursor() override {}
		void SetVSync(bool enabled) override {}
		void SetFullScreen(bool enabled) override {}

		bool IsFocused() const override { return false; }
		bool IsMinimized() const override { return false; }
		bool IsMaximized() const override { return false; }
		bool IsCursorEnabled() const override { return false; }
		bool IsVSync() const override { return false; }
		bool IsFullscreen() const override { return false; }

		void* GetNativeWindow() const override { return m_WindowHandle; }

		struct WindowData
		{
			std::string Title;
			uint32_t Width, Height;
			int32_t Xpos, Ypos;

			bool Minimized = false;
			bool Maximized = false;
			double CursorXpos, CursorYpos;

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
