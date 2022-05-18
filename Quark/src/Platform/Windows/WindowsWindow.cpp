#include "qkpch.h"
#include "WindowsWindow.h"

#include <Windows.h>
#include <windowsx.h>

// Conflicts with WindowsWindow::IsMinimized()/IsMaximized()
// From Windows.h
#undef IsMinimized(hwnd)
#undef IsMaximized(hwnd)

namespace Quark {

	static constexpr wchar_t s_ClassName[] = L"QuarkApp";
	static uint32_t s_WindowCount = 0;

	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static void OnWindowMoved(HWND hWnd, int32_t x, int32_t y);
	static void OnWindowSizeChanged(HWND hWnd, WPARAM wParam, LPARAM lParam);

	static std::wstring ConvertToWideString(const std::string& narrow)
	{
		std::wstring wide;
		wide.reserve(narrow.size());

		for (char c : narrow)
			wide.push_back(c);

		return wide;
	}

	using WindowData = WindowsWindow::WindowData;

	WindowsWindow::WindowsWindow(const WindowSpecification& spec)
	{
		QK_PROFILE_FUNCTION();

		if (s_WindowCount == 0)
			Init();

		m_Data.Title = spec.Title;
		m_WindowHandle = CreateWindowEx(
			0,                                           // Optional window styles.
			s_ClassName,                                 // Window class
			ConvertToWideString(spec.Title).c_str(),     // Window text
			WS_OVERLAPPEDWINDOW | WS_VISIBLE,            // Window style

			// Size and position
			CW_USEDEFAULT, CW_USEDEFAULT, spec.Width, spec.Height,

			NULL,       // Parent window    
			NULL,       // Menu
			NULL,       // Instance handle
			&m_Data     // Additional application data
		);

		QK_CORE_ASSERT(m_WindowHandle, "Window handle is nullptr");
		++s_WindowCount;

		m_Context = GraphicsContext::Create(m_WindowHandle);
		m_Context->Init();
	}

	WindowsWindow::~WindowsWindow()
	{
		QK_PROFILE_FUNCTION();

		m_Context.reset();
		DestroyWindow(m_WindowHandle);

		--s_WindowCount;

		if (s_WindowCount == 0)
			Shutdown();
	}

	void WindowsWindow::OnUpdate()
	{
		m_Context->SwapBuffers();

		MSG msg{};
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) > 0)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	void WindowsWindow::SetTitle(const std::string& title)
	{
		SetWindowText(m_WindowHandle, ConvertToWideString(title).c_str());
		m_Data.Title = title;
	}

	void WindowsWindow::AppendTitle(const std::string& title)
	{
		SetTitle(m_Data.Title + title);
	}

	void WindowsWindow::Focus()
	{
		SetFocus(m_WindowHandle);
	}

	void WindowsWindow::Minimize()
	{
		ShowWindow(m_WindowHandle, SW_MINIMIZE);
	}

	void WindowsWindow::Maximize()
	{
		ShowWindow(m_WindowHandle, SW_MAXIMIZE);
	}

	void WindowsWindow::Restore()
	{
		ShowWindow(m_WindowHandle, SW_RESTORE);
	}

	void WindowsWindow::RequestAttention()
	{
		FlashWindow(m_WindowHandle, TRUE);
	}

	void WindowsWindow::SetVSync(bool enabled)
	{
	}

	bool WindowsWindow::IsFocused() const
	{
		return GetFocus() == m_WindowHandle;
	}

	bool WindowsWindow::IsMinimized() const
	{
		return IsIconic(m_WindowHandle);
	}

	bool WindowsWindow::IsMaximized() const
	{
		return IsZoomed(m_WindowHandle);
	}

	void WindowsWindow::Init()
	{
		HINSTANCE hInstance = GetModuleHandle(NULL);

		WNDCLASS wc{};
		wc.lpfnWndProc   = WindowProc;
		wc.hInstance     = hInstance;
		wc.lpszClassName = s_ClassName;
		wc.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;

		RegisterClass(&wc);
	}

	void WindowsWindow::Shutdown()
	{
		HINSTANCE hInstance = GetModuleHandle(NULL);
		UnregisterClass(s_ClassName, hInstance);
	}

	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		// NOTE: Mouse movement event are currently unsupported
		// TODO: register input devices

		switch (uMsg)
		{
			case WM_CREATE:
			{
				CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
				// store the pointer in the instance data of the window
				// so it could always be retrieved by using GetWindowLongPtr(hwnd, GWLP_USERDATA)
				SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pCreate->lpCreateParams);

				EnableNonClientDpiScaling(hWnd);

				WindowData& data = *(WindowData*)pCreate->lpCreateParams;

				WINDOWPLACEMENT p{};
				GetWindowPlacement(hWnd, &p);

				data.Xpos = p.rcNormalPosition.left;
				data.Ypos = p.rcNormalPosition.top;

				break;
			}

			case WM_CLOSE:
			{
				WindowData& data = *(WindowData*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

				WindowClosedEvent event;
				if (data.EventCallback)
					data.EventCallback(event);

				break;
			}

			case WM_DESTROY:
			{
				PostQuitMessage(0);
				return 0;
			}

			case WM_SIZE:
			{
				OnWindowSizeChanged(hWnd, wParam, lParam);
				break;
			}

			case WM_MOVE:
			{
				OnWindowMoved(hWnd, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
				break;
			}

			case WM_SETFOCUS:
			{
				WindowData& data = *(WindowData*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

				WindowFocusedEvent event;
				if (data.EventCallback)
					data.EventCallback(event);

				break;
			}

			case WM_KILLFOCUS:
			{
				WindowData& data = *(WindowData*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

				WindowLostFocusEvent event;
				if (data.EventCallback)
					data.EventCallback(event);

				break;
			}

			case WM_MOUSEWHEEL:
			{
				WindowData& data = *(WindowData*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

				SHORT scroll = (SHORT)HIWORD(wParam) / WHEEL_DELTA;
				MouseScrolledEvent event(scroll, 0.0f);
				if (data.EventCallback)
					data.EventCallback(event);

				break;
			}

			case WM_MOUSEHWHEEL:
			{
				WindowData& data = *(WindowData*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

				SHORT scroll = -(SHORT)HIWORD(wParam) / WHEEL_DELTA;
				MouseScrolledEvent event(0.0f, scroll);
				if (data.EventCallback)
					data.EventCallback(event);

				break;
			}
		}

		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	void OnWindowMoved(HWND hWnd, int32_t x, int32_t y)
	{
		WindowData& data = *(WindowData*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

		WINDOWPLACEMENT p{};
		GetWindowPlacement(hWnd, &p);

		int32_t xOffset = p.rcNormalPosition.left - data.Xpos;
		int32_t yOffset = p.rcNormalPosition.top - data.Ypos;

		data.Xpos = p.rcNormalPosition.left;
		data.Ypos = p.rcNormalPosition.top;

		WindowMovedEvent event(data.Xpos, data.Ypos, xOffset, yOffset);
		if (data.EventCallback)
			data.EventCallback(event);
	}

	void OnWindowSizeChanged(HWND hWnd, WPARAM wParam, LPARAM lParam)
	{
		WindowData& data = *(WindowData*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

		bool minimized = wParam == SIZE_MINIMIZED;
		bool maximized = wParam == SIZE_MAXIMIZED;

		if ((data.Minimized) && wParam == SIZE_RESTORED)
		{
			WindowRestoredEvent event;
			if (data.EventCallback)
				data.EventCallback(event);
		}
		else
		{
			switch (wParam)
			{
				case SIZE_MINIMIZED:
				{
					WindowMinimizedEvent event;
					if (data.EventCallback)
						data.EventCallback(event);

					break;
				}

				case SIZE_MAXIMIZED:
				{
					WindowMaximizedEvent event;
					if (data.EventCallback)
						data.EventCallback(event);

					break;
				}

				default:
					break;
			}
		}

		UINT width = LOWORD(lParam);
		UINT height = HIWORD(lParam);

		data.Width = width;
		data.Height = height;

		WindowResizedEvent event(data.Width, data.Height);
		if (data.EventCallback)
			data.EventCallback(event);

		data.Minimized = minimized;
		data.Maximized = maximized;
	}
}
