#include "qkpch.h"
#include "GenericWindow.h"

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

	GenericWindow::GenericWindow(const WindowSpecification& spec)
	{
		Init(spec);
	}

	GenericWindow::~GenericWindow()
	{
		Shutdown();
	}

	void GenericWindow::OnUpdate()
	{
		m_Data.Context->SwapBuffers();
		glfwPollEvents();
	}

	void GenericWindow::Init(const WindowSpecification& spec)
	{
		QK_PROFILE_FUNCTION();

		if (s_WindowCount == 0)
		{
			int initCode = glfwInit();
			Verify(initCode == GLFW_TRUE, "Could not initialize GLFW!");

			QK_CORE_TRACE("Initialized GLFW!");

			glfwSetErrorCallback(GLFWErrorCallback);
			SetContextRelatedHints();
		}

		m_Data.Title   = spec.Title;
		m_Data.Width   = spec.Width;
		m_Data.Height  = spec.Height;
		m_Data.VSync   = spec.VSync;

		// MSAA anti-aliasing
		glfwWindowHint(GLFW_SAMPLES, GetIntegerSampleCount(spec.Samples));

		m_Window = glfwCreateWindow(spec.Width, spec.Height, spec.Title.c_str(), nullptr, nullptr);
		Verify(m_Window, "Failed to create window!");

		++s_WindowCount;

		// Creating the graphics context
		{
			m_Data.Context = GraphicsContext::Create(m_Window);

			SwapSurfaceFormat targetSurfaceFormat{};
			targetSurfaceFormat.Format     = ColorFormat::BGRA8SRGB;
			targetSurfaceFormat.ColorSpace = ColorSpace::SRGBNonLinear;

			uint32_t          bufferCount   = m_Data.Context->QuerySwapChainImageCount();
			ViewportExtent    swapExtent    = m_Data.Context->QuerySwapExtent();
			SwapPresentMode   presentMode   = m_Data.Context->ChooseSwapPresentMode(SwapPresentMode::Mailbox);
			SwapSurfaceFormat surfaceFormat = m_Data.Context->ChooseSurfaceFormat(targetSurfaceFormat);

			SwapChainSpecification swapChainSpec;
			swapChainSpec.MinImageCount     = bufferCount;
			swapChainSpec.Width             = swapExtent.Width;
			swapChainSpec.Height            = swapExtent.Height;
			swapChainSpec.SurfaceFormat     = surfaceFormat;
			swapChainSpec.PresentMode       = presentMode;
			swapChainSpec.Samples           = spec.Samples;
			swapChainSpec.DepthBufferFormat = ColorFormat::Depth32f;

			m_Data.Context->Init(swapChainSpec);
			m_Data.Context->SetSwapInterval(m_Data.VSync);
		}

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

			WindowResizedEvent event(data.Width, data.Height);
			data.EventCallback(event);
		});

		glfwSetFramebufferSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			uint32_t viewportWidth = (uint32_t)width;
			uint32_t viewportHeight = (uint32_t)height;

			data.Context->Resize(viewportWidth, viewportHeight);

			ViewportResizedEvent event(viewportWidth, viewportHeight);
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

	void GenericWindow::Shutdown()
	{
		QK_PROFILE_FUNCTION();

		m_Data.Context.reset();
		glfwDestroyWindow(m_Window);
		--s_WindowCount;

		if (s_WindowCount == 0)
			glfwTerminate();
	}

	Window& GenericWindow::SetTitle(std::string title)
	{
		m_Data.Title = std::move(title);
		glfwSetWindowTitle(m_Window, m_Data.Title.c_str());
		return *this;
	}

	Window& GenericWindow::AppendTitle(std::string title)
	{
		m_Data.Title.append(std::move(title));
		glfwSetWindowTitle(m_Window, m_Data.Title.c_str());
		return *this;
	}

	void GenericWindow::Resize(uint32_t width, uint32_t height)
	{
		glfwSetWindowSize(m_Window, width, height);
	}

	void GenericWindow::Focus()
	{
		glfwFocusWindow(m_Window);
	}

	void GenericWindow::Minimize()
	{
		glfwIconifyWindow(m_Window);
	}

	void GenericWindow::Maximize()
	{
		glfwMaximizeWindow(m_Window);
	}

	void GenericWindow::Restore()
	{
		glfwRestoreWindow(m_Window);
	}

	void GenericWindow::RequestAttention()
	{
		glfwRequestWindowAttention(m_Window);
	}

	void GenericWindow::DisableCursor()
	{
		glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}

	void GenericWindow::EnableCursor()
	{
		glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

	void GenericWindow::SetVSync(bool enabled)
	{
		m_Data.Context->SetSwapInterval(enabled);
		m_Data.VSync = enabled;
	}

	void GenericWindow::SetFullscreen(bool enabled)
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
			m_Data.Context->SetSwapInterval(m_Data.VSync);
		}
		else
		{
			m_Data.Width = m_Data.WindowedWidth;
			m_Data.Height = m_Data.WindowedHeight;

			// Restore last window size and position
			glfwSetWindowMonitor(m_Window, nullptr, m_Data.WindowedPosX, m_Data.WindowedPosY, m_Data.Width, m_Data.Height, 0);
		}
	}

	bool GenericWindow::IsFocused() const
	{
		return glfwGetWindowAttrib(m_Window, GLFW_FOCUSED);
	}

	bool GenericWindow::IsMinimized() const
	{
		return glfwGetWindowAttrib(m_Window, GLFW_ICONIFIED);
	}

	bool GenericWindow::IsMaximized() const
	{
		return glfwGetWindowAttrib(m_Window, GLFW_MAXIMIZED);
	}

	bool GenericWindow::IsCursorEnabled() const
	{
		return glfwGetInputMode(m_Window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL;
	}

	bool GenericWindow::IsFullscreen() const
	{
		return glfwGetWindowMonitor(m_Window) != nullptr;
	}
}
