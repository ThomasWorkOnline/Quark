#pragma once

#include "../../Entropy/Core/Window.h"
#include "../../Entropy/Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace Entropy {

	class GenericWindow : public Window
	{
	public:
		GenericWindow(uint32_t width, uint32_t height, const char* title);
		virtual ~GenericWindow();

		virtual void OnUpdate() override;

		virtual void EnableFullScreen() override;
		virtual void DisableFullScreen() override;

		virtual uint32_t GetWidth() const override { return m_Data.Width; }
		virtual uint32_t GetHeight() const override { return m_Data.Height; }

		virtual const std::string& GetTitle() const override { return m_Data.Title; }
		virtual void SetTitle(const std::string& title) override;
		virtual void AppendTitle(const std::string& title) override;

		virtual void Select() override;
		virtual void Deselect() override;
		virtual bool IsSelected() const override { return IsCursorDisabled(); }
		virtual bool IsCursorDisabled() const override;
		virtual bool IsCursorNormal() const override;
		virtual bool IsFullscreen() const override;

		virtual void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
		virtual void SetVSync(bool enabled) override;
		virtual bool IsVSync() const override { return m_Data.VSync; }

		virtual void* GetNativeWindow() const override { return m_Window; }

	private:
		virtual void Init(uint32_t width, uint32_t height, const char* title);
		virtual void Shutdown();

		struct WindowData
		{
			std::string Title = "";
			uint32_t Width = 0, Height = 0;
			bool VSync = true;

			EventCallbackFn EventCallback;
		};

		Scope<GraphicsContext> m_Context;
		GLFWwindow* m_Window;
		WindowData m_Data;
	};
}
