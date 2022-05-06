#pragma once

#include "Quark/Core/Window.h"
#include "Quark/Renderer/GraphicsContext.h"

typedef struct GLFWwindow GLFWwindow;

namespace Quark {

	class GLFWWindow : public Window
	{
	public:
		GLFWWindow(const WindowSpecification& spec);
		~GLFWWindow() override;

		void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
		void OnUpdate() override;

		uint32_t GetWidth() const override { return m_Data.Width; }
		uint32_t GetHeight() const override { return m_Data.Height; }
		const std::string& GetTitle() const override { return m_Data.Title; }

		void SetTitle(const std::string& title) override;
		void AppendTitle(const std::string& title) override;

		void Focus() override;
		void Minimize() override;
		void Maximize() override;
		void Restore() override;
		void RequestAttention() override;
		void DisableCursor() override;
		void EnableCursor() override;
		void SetVSync(bool enabled) override;
		void SetFullScreen(bool enabled) override;

		bool IsFocused() const override;
		bool IsMinimized() const override;
		bool IsMaximized() const override;
		bool IsCursorEnabled() const override;
		bool IsVSync() const override { return m_Data.VSync; }
		bool IsFullscreen() const override;

		void* GetNativeWindow() const override { return m_Window; }

	private:
		void Init(const WindowSpecification& spec);
		void Shutdown();

		struct WindowData
		{
			std::string Title;
			uint32_t Samples;

			uint32_t Width, Height;
			int32_t Xpos, Ypos;

			bool VSync;
			double CursorXpos, CursorYpos;

			EventCallbackFn EventCallback;
		};

		Scope<GraphicsContext> m_Context;
		GLFWwindow* m_Window;
		WindowData m_Data{};

		int32_t m_WindowedPosX = 0, m_WindowedPosY = 0;
		int32_t m_WindowedWidth = 0, m_WindowedHeight = 0;
	};
}
