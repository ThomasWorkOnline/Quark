#include "WindowsWindow.h"

#include "../../Entropy/Renderer/Renderer.h"
#include "../../Entropy/Core/Application.h"

#include "../../Platform/OpenGL/OpenGLGraphicsContext.h"

#include <GLFW/glfw3.h>

namespace Entropy {

	static uint32_t s_WindowCount = 0;
	static int32_t s_WindowedPosX = 0, s_WindowedPosY = 0;
	static int32_t s_WindowedWidth = 0, s_WindowedHeight = 0;

	static void GLFWErrorCallback(int32_t error, const char* description)
	{
		NT_FATAL(description);
	}

	WindowsWindow::WindowsWindow(uint32_t width, uint32_t height, const char* title)
	{
		Init(width, height, title);
	}

	WindowsWindow::~WindowsWindow()
	{
		Shutdown();
	}

	void WindowsWindow::EnableFullScreen()
	{
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);

		glfwGetWindowPos(m_Window, &s_WindowedPosX, &s_WindowedPosY);
		glfwGetWindowSize(m_Window, &s_WindowedWidth, &s_WindowedHeight);

		// Switch to full screen
		glfwSetWindowMonitor(m_Window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
	}

	void WindowsWindow::DisableFullScreen()
	{
		m_Data.Width = s_WindowedWidth;
		m_Data.Height = s_WindowedHeight;

		// Restore last window size and position
		glfwSetWindowMonitor(m_Window, nullptr, s_WindowedPosX, s_WindowedPosY, (int32_t)m_Data.Width, (int32_t)m_Data.Height, 0);
	}

	void WindowsWindow::OnUpdate()
	{
		glfwPollEvents();
		m_Context->SwapBuffers();
	}

	void WindowsWindow::Init(uint32_t width, uint32_t height, const char* title)
	{
		m_Data.Title = title;
		m_Data.Width = width;
		m_Data.Height = height;

		if (s_WindowCount == 0)
		{
			if (!glfwInit())
				NT_FATAL("Could not init GLFW");
			NT_TRACE("Initialized GLFW successfully!");

			glfwSetErrorCallback(GLFWErrorCallback);

			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
			glfwWindowHint(GLFW_AUTO_ICONIFY, GLFW_TRUE);

			// MSAA anti-aliasing
			glfwWindowHint(GLFW_SAMPLES, 4);
		}

		m_Window = glfwCreateWindow((int32_t)m_Data.Width, (int32_t)m_Data.Height, m_Data.Title.c_str(), nullptr, nullptr);

		++s_WindowCount;

		// Creating the graphics context
		m_Context = GraphicsContext::Create(m_Window);
		m_Context->Init();

		glfwSetWindowUserPointer(m_Window, &m_Data);

		// Experimental
		Select();

		// -----------------------------------------------------------------------
		// CALLBACKS
		// -----------------------------------------------------------------------
		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int32_t width, int32_t height)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				data.Width = width;
				data.Height = height;

				WindowResizeEvent event(width, height);
				data.EventCallback(event);
			});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				WindowCloseEvent event;
				data.EventCallback(event);
			});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int32_t key, int32_t scancode, int32_t action, int32_t mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				switch (action)
				{
				case GLFW_PRESS:
				{
					KeyPressedEvent event(static_cast<KeyCode>(key), 0);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					KeyReleasedEvent event(static_cast<KeyCode>(key));
					data.EventCallback(event);
					break;
				}
				case GLFW_REPEAT:
				{
					KeyPressedEvent event(static_cast<KeyCode>(key), 1);
					data.EventCallback(event);
					break;
				}
				}
			});

		glfwSetCharCallback(m_Window, [](GLFWwindow* window, uint32_t keycode)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				KeyTypedEvent event(static_cast<KeyCode>(keycode));
				data.EventCallback(event);
			});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int32_t button, int32_t action, int32_t mods)
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

				MouseMovedEvent event((float)xPos, (float)yPos);
				data.EventCallback(event);
			});
	}

	void WindowsWindow::Shutdown()
	{
		glfwDestroyWindow(m_Window);
		--s_WindowCount;

		// Cleans up glfw's garbage
		if (s_WindowCount == 0)
			glfwTerminate();
	}

	void WindowsWindow::SetTitle(const char* title)
	{
		m_Data.Title = title;
		glfwSetWindowTitle(m_Window, title);
	}

	void WindowsWindow::SetTitle(const std::string& title)
	{
		m_Data.Title = title;
		glfwSetWindowTitle(m_Window, title.c_str());
	}

	void WindowsWindow::AppendTitle(const std::string& title)
	{
		SetTitle(m_Data.Title + title);
	}

	void WindowsWindow::Select()
	{
		glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}

	void WindowsWindow::Deselect()
	{
		glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

	bool WindowsWindow::IsCursorDisabled() const
	{
		return glfwGetInputMode(m_Window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED;
	}

	bool WindowsWindow::IsCursorNormal() const
	{
		return glfwGetInputMode(m_Window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL;
	}

	bool WindowsWindow::IsFullscreen() const
	{
		return glfwGetWindowMonitor(m_Window) != nullptr;
	}

	void WindowsWindow::SetVSync(bool enabled)
	{
		glfwSwapInterval(enabled);
		m_Data.VSync = enabled;
	}
}
