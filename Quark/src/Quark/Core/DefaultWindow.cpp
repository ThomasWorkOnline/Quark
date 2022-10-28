#include "qkpch.h"
#include "DefaultWindow.h"

#include "Quark/Renderer/GraphicsAPI.h"

#include <GLFW/glfw3.h>

namespace Quark {

	static void SetContextRelatedHints()
	{
		if (GraphicsAPI::GetAPI() == RHI::OpenGL)
		{
			glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
			glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

#if defined(QK_PLATFORM_MACOS)
			// Max supported OpenGL version on modern macOS
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
#else
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
#endif

#ifdef QK_DEBUG
			glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif
		}
		else
		{
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		}

#if defined(QK_PLATFORM_MACOS)
		glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_TRUE);
		glfwWindowHint(GLFW_COCOA_GRAPHICS_SWITCHING, GLFW_TRUE);
		QK_CORE_TRACE("Set macOS specific window hints!");
#endif
	}

	static uint32_t s_WindowCount = 0;

	static void GLFWErrorCallback(int32_t error, const char* description)
	{
		if (error & GLFW_FORMAT_UNAVAILABLE)
			return;

		QK_CORE_ASSERT(false, description);
	}

	DefaultWindow::DefaultWindow(const WindowSpecification& spec)
	{
		Init(spec);
	}

	DefaultWindow::~DefaultWindow()
	{
		Shutdown();
	}

	void DefaultWindow::OnUpdate()
	{
		m_Data.Context->SwapBuffers();
		glfwPollEvents();
	}

	void DefaultWindow::Init(const WindowSpecification& spec)
	{
		QK_PROFILE_FUNCTION();

		if (s_WindowCount == 0)
		{
			int initCode = glfwInit();
			QK_CORE_ASSERT(initCode == GLFW_TRUE, "Could not initialize GLFW!");
			QK_CORE_TRACE("Initialized GLFW!");

			glfwSetErrorCallback(GLFWErrorCallback);
			SetContextRelatedHints();
		}

		m_Data.Title   = spec.Title;
		m_Data.Samples = spec.Samples;
		m_Data.Width   = spec.Width;
		m_Data.Height  = spec.Height;
		m_Data.VSync   = spec.VSync;

		// MSAA anti-aliasing
		glfwWindowHint(GLFW_SAMPLES, m_Data.Samples);

		m_Window = glfwCreateWindow(spec.Width, spec.Height, spec.Title.c_str(), nullptr, nullptr);
		QK_CORE_ASSERT(m_Window, "Failed to create window!");

		++s_WindowCount;

		// Creating the graphics context
		m_Data.Context = GraphicsContext::Create(m_Window);
		m_Data.Context->Init();
		m_Data.Context->SetSwapInterval(m_Data.VSync);

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
				data.Width = (uint32_t)width;
				data.Height = (uint32_t)height;

				data.Context->Resize(data.Width, data.Height);

				WindowResizedEvent event(data.Width, data.Height);
				data.EventCallback(event);
			});

		glfwSetFramebufferSizeCallback(m_Window, [](GLFWwindow* window, int viewportWidth, int viewportHeight)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				ViewportResizedEvent event((uint32_t)viewportWidth, (uint32_t)viewportHeight);
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
				ModifierKey modKeys = static_cast<ModifierKey>(mods);

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

	void DefaultWindow::Shutdown()
	{
		QK_PROFILE_FUNCTION();

		m_Data.Context.reset();
		glfwDestroyWindow(m_Window);
		--s_WindowCount;

		if (s_WindowCount == 0)
			glfwTerminate();
	}

	Window& DefaultWindow::SetTitle(std::string title)
	{
		m_Data.Title = std::move(title);
		glfwSetWindowTitle(m_Window, m_Data.Title.c_str());
		return *this;
	}

	Window& DefaultWindow::AppendTitle(std::string title)
	{
		m_Data.Title.append(std::move(title));
		glfwSetWindowTitle(m_Window, m_Data.Title.c_str());
		return *this;
	}

	void DefaultWindow::Resize(uint32_t width, uint32_t height)
	{
		glfwSetWindowSize(m_Window, width, height);
	}

	void DefaultWindow::Focus()
	{
		glfwFocusWindow(m_Window);
	}

	void DefaultWindow::Minimize()
	{
		glfwIconifyWindow(m_Window);
	}

	void DefaultWindow::Maximize()
	{
		glfwMaximizeWindow(m_Window);
	}

	void DefaultWindow::Restore()
	{
		glfwRestoreWindow(m_Window);
	}

	void DefaultWindow::RequestAttention()
	{
		glfwRequestWindowAttention(m_Window);
	}

	void DefaultWindow::DisableCursor()
	{
		glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}

	void DefaultWindow::EnableCursor()
	{
		glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

	void DefaultWindow::SetVSync(bool enabled)
	{
		m_Data.Context->SetSwapInterval(enabled);
		m_Data.VSync = enabled;
	}

	void DefaultWindow::SetFullscreen(bool enabled)
	{
		bool isFullScreen = glfwGetWindowMonitor(m_Window) != nullptr;
		if (isFullScreen == enabled) return;

		if (enabled)
		{
			GLFWmonitor* monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode* mode = glfwGetVideoMode(monitor);

			glfwGetWindowPos(m_Window, &m_Data.WindowedPosX, &m_Data.WindowedPosY);
			glfwGetWindowSize(m_Window, &m_Data.WindowedWidth, &m_Data.WindowedHeight);

			// Switch to full screen
			glfwSetWindowMonitor(m_Window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
		}
		else
		{
			m_Data.Width = m_Data.WindowedWidth;
			m_Data.Height = m_Data.WindowedHeight;

			// Restore last window size and position
			glfwSetWindowMonitor(m_Window, nullptr, m_Data.WindowedPosX, m_Data.WindowedPosY, m_Data.Width, m_Data.Height, 0);
		}
	}

	bool DefaultWindow::IsFocused() const
	{
		return glfwGetWindowAttrib(m_Window, GLFW_FOCUSED);
	}

	bool DefaultWindow::IsMinimized() const
	{
		return glfwGetWindowAttrib(m_Window, GLFW_ICONIFIED);
	}

	bool DefaultWindow::IsMaximized() const
	{
		return glfwGetWindowAttrib(m_Window, GLFW_MAXIMIZED);
	}

	bool DefaultWindow::IsCursorEnabled() const
	{
		return glfwGetInputMode(m_Window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL;
	}

	bool DefaultWindow::IsFullscreen() const
	{
		return glfwGetWindowMonitor(m_Window) != nullptr;
	}

	const char* DefaultWindow::GetClipboardText() const
	{
		return glfwGetClipboardString(m_Window);
	}

	void DefaultWindow::SetClipboardText(const char* string)
	{
		glfwSetClipboardString(m_Window, string);
	}
}
