#pragma once

#include "Quark/Core/Window.h"
#include "Quark/Renderer/GraphicsContext.h"

typedef struct GLFWwindow GLFWwindow;

namespace Quark {

	class GLFWWindow final : public Window
	{
	public:
		GLFWWindow(const WindowSpecification& spec);
		virtual ~GLFWWindow() final override;

		virtual void SetEventCallback(const EventCallbackFn& callback) final override { m_Data.EventCallback = callback; }
		virtual void OnUpdate() final override;

		virtual uint32_t GetWidth() const final override { return m_Data.Width; }
		virtual uint32_t GetHeight() const final override { return m_Data.Height; }
		virtual std::string_view GetTitle() const final override { return m_Data.Title; }

		virtual Window& SetTitle(std::string title) final override;
		virtual Window& AppendTitle(std::string title) final override;

		virtual void Resize(uint32_t width, uint32_t height) final override;

		virtual void Focus() final override;
		virtual void Minimize() final override;
		virtual void Maximize() final override;
		virtual void Restore() final override;
		virtual void RequestAttention() final override;
		virtual void DisableCursor() final override;
		virtual void EnableCursor() final override;
		virtual void SetVSync(bool enabled) final override;
		virtual void SetFullscreen(bool enabled) final override;

		virtual bool IsFocused() const final override;
		virtual bool IsMinimized() const final override;
		virtual bool IsMaximized() const final override;
		virtual bool IsCursorEnabled() const final override;
		virtual bool IsVSync() const final override { return m_Data.VSync; }
		virtual bool IsFullscreen() const final override;

		virtual void* GetNativeWindow() const final override { return m_Window; }

		// Non-Copyable
		GLFWWindow(const GLFWWindow&) = delete;
		GLFWWindow& operator=(const GLFWWindow&) = delete;

		// Non-Movable
		GLFWWindow(GLFWWindow&&) = delete;
		GLFWWindow& operator=(GLFWWindow&&) = delete;

	private:
		void Init(const WindowSpecification& spec);
		void Shutdown();

		struct WindowData
		{
			std::string Title;
			uint32_t    Samples;

			uint32_t    Width, Height;
			int32_t     Xpos, Ypos;

			bool        VSync;
			double      CursorXpos, CursorYpos;

			Scope<GraphicsContext> Context;
			EventCallbackFn EventCallback;
		};

		GLFWwindow* m_Window;
		WindowData m_Data{};

		int32_t m_WindowedPosX = 0, m_WindowedPosY = 0;
		int32_t m_WindowedWidth = 0, m_WindowedHeight = 0;
	};
}
