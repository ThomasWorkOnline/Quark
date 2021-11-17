#pragma once

#include "../../Quark/Core/Window.h"
#include "../../Quark/Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace Quark {

	class GLFWWindow : public Window
	{
	public:
		GLFWWindow(const WindowSpecification& spec);
		virtual ~GLFWWindow();

		virtual void OnUpdate() override;

		virtual uint32_t GetWidth() const override { return m_Data.Width; }
		virtual uint32_t GetHeight() const override { return m_Data.Height; }

		virtual const std::string& GetTitle() const override { return m_Data.Title; }
		virtual void SetTitle(const std::string& title) override;
		virtual void AppendTitle(const std::string& title) override;

		virtual void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }

		virtual void Select() override;
		virtual void Deselect() override;
		virtual bool IsSelected() const override { return IsCursorDisabled(); }
		virtual bool IsCursorDisabled() const override;
		virtual bool IsCursorNormal() const override;
		virtual void SetVSync(bool enabled) override;
		virtual bool IsVSync() const override { return m_Data.VSync; }
		virtual void SetFullScreen(bool enabled) override;
		virtual bool IsFullscreen() const override;

		virtual void* GetNativeWindow() const override { return m_Window; }

	private:
		void Init(const WindowSpecification& spec);
		void Shutdown();

		struct WindowData
		{
			std::string Title;
			uint32_t Width, Height;
			uint32_t Samples;
			bool VSync;

			double LastXPos = 0, LastYPos = 0;

			EventCallbackFn EventCallback;
		};

		Scope<GraphicsContext> m_Context;
		GLFWwindow* m_Window;
		WindowData m_Data;

		int32_t m_WindowedPosX = 0, m_WindowedPosY = 0;
		int32_t m_WindowedWidth = 0, m_WindowedHeight = 0;
	};
}
