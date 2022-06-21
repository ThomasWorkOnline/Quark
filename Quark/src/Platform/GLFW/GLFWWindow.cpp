#include "qkpch.h"
#include "GLFWWindow.h"

#include "Quark/Renderer/GraphicsAPI.h"
#include <GLFW/glfw3.h>

namespace Quark {

	static constexpr ModifierKey GetModKey(int mod)
	{
		uint8_t keys{};
		switch (mod)
		{
			// TODO: rethink this when multiple mod keys are pressed
			case GLFW_MOD_SHIFT:     keys = (uint8_t)keys | (uint8_t)ModifierKey::Shift;    break;
			case GLFW_MOD_CONTROL:   keys = (uint8_t)keys | (uint8_t)ModifierKey::Control;  break;
			case GLFW_MOD_ALT:       keys = (uint8_t)keys | (uint8_t)ModifierKey::Alt;      break;
			case GLFW_MOD_SUPER:     keys = (uint8_t)keys | (uint8_t)ModifierKey::Super;    break;
			case GLFW_MOD_CAPS_LOCK: keys = (uint8_t)keys | (uint8_t)ModifierKey::CapsLock; break;
			case GLFW_MOD_NUM_LOCK:  keys = (uint8_t)keys | (uint8_t)ModifierKey::NumLock;  break;
		}

		return static_cast<ModifierKey>(keys);
	}

	static uint32_t s_WindowCount = 0;

	static void GLFWErrorCallback(int32_t error, const char* description)
	{
		QK_CORE_FATAL(description);
	}

	GLFWWindow::GLFWWindow(const WindowSpecification& spec)
	{
		Init(spec);
	}

	GLFWWindow::~GLFWWindow()
	{
		Shutdown();
	}

	void GLFWWindow::OnUpdate()
	{
		m_Context->SwapBuffers();
		glfwPollEvents();
	}

	void GLFWWindow::Init(const WindowSpecification& spec)
	{
		QK_PROFILE_FUNCTION();

		m_Data.Width	= spec.Width;
		m_Data.Height	= spec.Height;
		m_Data.Title	= spec.Title;
		m_Data.Samples	= spec.Samples;

		if (s_WindowCount == 0)
		{
			int initCode = glfwInit();
			QK_CORE_ASSERT(initCode == GLFW_TRUE, "Could not initialize GLFW!");

			glfwSetErrorCallback(GLFWErrorCallback);
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

			if (GraphicsAPI::GetAPI() == GraphicsAPI::API::OpenGL)
			{
				glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);

#ifdef QK_DEBUG
				glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT,     GLFW_TRUE);
#endif

#if defined(QK_PLATFORM_MACOS)
				glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,    3);
				glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,    2);
				glfwWindowHint(GLFW_OPENGL_PROFILE,           GLFW_OPENGL_CORE_PROFILE);
				glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,    GLFW_TRUE);
#endif
			}

#if defined(QK_PLATFORM_MACOS)
			glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_TRUE);
			glfwWindowHint(GLFW_COCOA_GRAPHICS_SWITCHING, GLFW_TRUE);
#endif
		}

		// MSAA anti-aliasing
		glfwWindowHint(GLFW_SAMPLES, m_Data.Samples);

		m_Window = glfwCreateWindow(m_Data.Width, m_Data.Height, m_Data.Title.c_str(), nullptr, nullptr);
		++s_WindowCount;

		// Creating the graphics context
		m_Context = GraphicsContext::Create(m_Window);
		m_Context->Init();

		glfwSetWindowUserPointer(m_Window, &m_Data);

		// Init window states in user ptr
		glfwGetCursorPos(m_Window, &m_Data.CursorXpos, &m_Data.CursorYpos);

		// -----------------------------------------------------------------------
		// CALLBACKS
		// -----------------------------------------------------------------------
		glfwSetWindowPosCallback(m_Window, [](GLFWwindow* window, int xpos, int ypos)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				int32_t xOffset = xpos - data.Xpos;
				int32_t yOffset = ypos - data.Ypos;

				data.Xpos = xpos;
				data.Ypos = ypos;

				WindowMovedEvent event(xpos, ypos, xOffset, yOffset);
				data.EventCallback(event);
			});

		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				data.Width = width;
				data.Height = height;

				WindowResizedEvent event(width, height);
				data.EventCallback(event);
			});

		glfwSetWindowIconifyCallback(m_Window, [](GLFWwindow* window, int iconified)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				if (iconified)
				{
					WindowMinimizedEvent event;
					data.EventCallback(event);
				}
				else
				{
					WindowRestoredEvent event;
					data.EventCallback(event);
				}
			});

		glfwSetWindowMaximizeCallback(m_Window, [](GLFWwindow* window, int maximized)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				if (maximized)
				{
					WindowMaximizedEvent event;
					data.EventCallback(event);
				}
				else
				{
					WindowRestoredEvent event;
					data.EventCallback(event);
				}
			});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				WindowClosedEvent event;
				data.EventCallback(event);
			});

		glfwSetWindowFocusCallback(m_Window, [](GLFWwindow* window, int focused)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				if (focused)
				{
					WindowFocusedEvent event;
					data.EventCallback(event);
				}
				else
				{
					WindowLostFocusEvent event;
					data.EventCallback(event);
				}
			});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				ModifierKey modKeys = GetModKey(mods);

				switch (action)
				{
					case GLFW_PRESS:
					{
						KeyPressedEvent event(static_cast<KeyCode>(key), modKeys, 0);
						data.EventCallback(event);
						break;
					}
					case GLFW_RELEASE:
					{
						KeyReleasedEvent event(static_cast<KeyCode>(key), modKeys);
						data.EventCallback(event);
						break;
					}
					case GLFW_REPEAT:
					{
						KeyPressedEvent event(static_cast<KeyCode>(key), modKeys, 1);
						data.EventCallback(event);
						break;
					}
				}
			});

		glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int keycode)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				KeyTypedEvent event(static_cast<KeyCode>(keycode));
				data.EventCallback(event);
			});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				switch (action)
				{
					case GLFW_PRESS:
					{
						MouseButtonPressedEvent event(static_cast<MouseCode>(button));
						data.EventCallback(event);
						break;
					}
					case GLFW_RELEASE:
					{
						MouseButtonReleasedEvent event(static_cast<MouseCode>(button));
						data.EventCallback(event);
						break;
					}
				}
			});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				MouseScrolledEvent event((float)xOffset, (float)yOffset);
				data.EventCallback(event);
			});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				double xOffset = xPos - data.CursorXpos;
				double yOffset = yPos - data.CursorYpos;

				data.CursorXpos = xPos;
				data.CursorYpos = yPos;

				MouseMovedEvent event((float)xPos, (float)yPos, (float)xOffset, (float)yOffset);
				data.EventCallback(event);
			});
	}

	void GLFWWindow::Shutdown()
	{
		QK_PROFILE_FUNCTION();

		m_Context.reset();
		glfwDestroyWindow(m_Window);
		--s_WindowCount;

		if (s_WindowCount == 0)
			glfwTerminate();
	}

	void GLFWWindow::SetTitle(const std::string& title)
	{
		glfwSetWindowTitle(m_Window, title.c_str());
		m_Data.Title = title;
	}

	void GLFWWindow::AppendTitle(const std::string& title)
	{
		SetTitle(m_Data.Title + title);
	}

	void GLFWWindow::Focus()
	{
		glfwFocusWindow(m_Window);
	}

	void GLFWWindow::Minimize()
	{
		glfwIconifyWindow(m_Window);
	}

	void GLFWWindow::Maximize()
	{
		glfwMaximizeWindow(m_Window);
	}

	void GLFWWindow::Restore()
	{
		glfwRestoreWindow(m_Window);
	}

	void GLFWWindow::RequestAttention()
	{
		glfwRequestWindowAttention(m_Window);
	}

	void GLFWWindow::DisableCursor()
	{
		glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}

	void GLFWWindow::EnableCursor()
	{
		glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

	void GLFWWindow::SetVSync(bool enabled)
	{
		glfwSwapInterval(enabled);
		m_Data.VSync = enabled;
	}

	void GLFWWindow::SetFullScreen(bool enabled)
	{
		if (IsFullscreen() == enabled) return;

		if (enabled)
		{
			GLFWmonitor* monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode* mode = glfwGetVideoMode(monitor);

			glfwGetWindowPos(m_Window, &m_WindowedPosX, &m_WindowedPosY);
			glfwGetWindowSize(m_Window, &m_WindowedWidth, &m_WindowedHeight);

			// Switch to full screen
			glfwSetWindowMonitor(m_Window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
		}
		else
		{
			m_Data.Width = m_WindowedWidth;
			m_Data.Height = m_WindowedHeight;

			// Restore last window size and position
			glfwSetWindowMonitor(m_Window, nullptr, m_WindowedPosX, m_WindowedPosY, m_Data.Width, m_Data.Height, 0);
		}
	}

	bool GLFWWindow::IsFocused() const
	{
		return glfwGetWindowAttrib(m_Window, GLFW_FOCUSED);
	}

	bool GLFWWindow::IsMinimized() const
	{
		return glfwGetWindowAttrib(m_Window, GLFW_ICONIFIED);
	}

	bool GLFWWindow::IsMaximized() const
	{
		return glfwGetWindowAttrib(m_Window, GLFW_MAXIMIZED);
	}

	bool GLFWWindow::IsCursorEnabled() const
	{
		return glfwGetInputMode(m_Window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL;
	}

	bool GLFWWindow::IsFullscreen() const
	{
		return glfwGetWindowMonitor(m_Window) != nullptr;
	}
}
