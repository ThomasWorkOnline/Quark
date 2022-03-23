#pragma once

#include "Quark/Core/Window.h"
#include "Quark/Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace Quark {

	class GLFWWindow : public Window
	{
	public:
		GLFWWindow(const WindowSpecification& spec);
		virtual ~GLFWWindow() override;

		virtual void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
		virtual void OnUpdate() override;

		virtual uint32_t GetWidth() const override { return m_Data.Width; }
		virtual uint32_t GetHeight() const override { return m_Data.Height; }
		virtual const std::string& GetTitle() const override { return m_Data.Title; }

		virtual void SetTitle(const std::string& title) override;
		virtual void AppendTitle(const std::string& title) override;

		virtual void Focus() override;
		virtual void Minimize() override;
		virtual void Maximize() override;
		virtual void Restore() override;
		virtual void RequestAttention() override;
		virtual void DisableCursor() override;
		virtual void EnableCursor() override;
		virtual void SetVSync(bool enabled) override;
		virtual void SetFullScreen(bool enabled) override;

		virtual bool IsFocused() const override;
		virtual bool IsMinimized() const override;
		virtual bool IsMaximized() const override;
		virtual bool IsCursorEnabled() const override;
		virtual bool IsVSync() const override { return m_Data.VSync; }
		virtual bool IsFullscreen() const override;

		virtual void* GetNativeWindow() const override { return m_Window; }

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
			double CursorXpos = 0, CursorYpos = 0;

			EventCallbackFn EventCallback;
		};

		Scope<GraphicsContext> m_Context;
		GLFWwindow* m_Window;
		WindowData m_Data{};

		int32_t m_WindowedPosX = 0, m_WindowedPosY = 0;
		int32_t m_WindowedWidth = 0, m_WindowedHeight = 0;
	};
}
