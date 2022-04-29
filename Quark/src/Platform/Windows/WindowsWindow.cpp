#include "qkpch.h"
#include "WindowsWindow.h"

#include <Windows.h>

namespace Quark {

	static constexpr wchar_t s_ClassName[] = L"QuarkApp";
	static uint32_t s_WindowCount = 0;

	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	WindowsWindow::WindowsWindow(const WindowSpecification& spec)
	{
		QK_PROFILE_FUNCTION();

		if (s_WindowCount == 0)
			Init();

		m_Data.Title  = spec.Title;

		m_WindowHandle = CreateWindowEx(
			0,                                           // Optional window styles.
			s_ClassName,                                 // Window class
			L"Quark",                                    // Window text
			WS_OVERLAPPEDWINDOW | WS_VISIBLE | CS_OWNDC, // Window style

			// Size and position
			CW_USEDEFAULT, CW_USEDEFAULT, spec.Width, spec.Height,

			NULL,       // Parent window    
			NULL,       // Menu
			NULL,       // Instance handle
			&m_Data     // Additional application data
		);

		++s_WindowCount;
		QK_CORE_ASSERT(m_WindowHandle, "Window handle is nullptr");

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
		if (GetMessage(&msg, NULL, 0, 0) > 0)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	void WindowsWindow::Init()
	{
		HINSTANCE hInstance = GetModuleHandle(NULL);

		WNDCLASS wc{};
		wc.lpfnWndProc   = WindowProc;
		wc.hInstance     = hInstance;
		wc.lpszClassName = s_ClassName;

		RegisterClass(&wc);
	}

	void WindowsWindow::Shutdown()
	{
		HINSTANCE hInstance = GetModuleHandle(NULL);
		UnregisterClass(s_ClassName, hInstance);
	}

	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		using WindowData = WindowsWindow::WindowData;

		// NOTE: Mouse movement event are currently unsupported
		// TODO: register input devices

		switch (uMsg)
		{
			case WM_CREATE:
			{
				CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
				// store the pointer in the instance data of the window
				// so it could always be retrieved by using GetWindowLongPtr(hwnd, GWLP_USERDATA)
				SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pCreate->lpCreateParams);

				WindowData& data = *(WindowData*)pCreate->lpCreateParams;

				WINDOWPLACEMENT p{};
				GetWindowPlacement(hwnd, &p);

				data.Xpos = p.rcNormalPosition.left;
				data.Ypos = p.rcNormalPosition.top;

				break;
			}

			case WM_CLOSE:
			{
				WindowData& data = *(WindowData*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

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
				WindowData& data = *(WindowData*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

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
						{
							UINT width = LOWORD(lParam);
							UINT height = HIWORD(lParam);

							data.Width = width;
							data.Height = height;

							WindowResizedEvent event(data.Width, data.Height);
							if (data.EventCallback)
								data.EventCallback(event);

							break;
						}
					}
				}

				data.Minimized = minimized;
				data.Maximized = maximized;

				break;
			}

			case WM_MOVE:
			{
				WindowData& data = *(WindowData*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

				WINDOWPLACEMENT p{};
				GetWindowPlacement(hwnd, &p);

				int32_t xOffset = p.rcNormalPosition.left - data.Xpos;
				int32_t yOffset = p.rcNormalPosition.top - data.Ypos;

				data.Xpos = p.rcNormalPosition.left;
				data.Ypos = p.rcNormalPosition.top;

				WindowMovedEvent event(data.Xpos, data.Ypos, xOffset, yOffset);
				if (data.EventCallback)
					data.EventCallback(event);

				break;
			}

			case WM_SETFOCUS:
			{
				WindowData& data = *(WindowData*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

				WindowFocusedEvent event;
				if (data.EventCallback)
					data.EventCallback(event);

				break;
			}

			case WM_KILLFOCUS:
			{
				WindowData& data = *(WindowData*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

				WindowLostFocusEvent event;
				if (data.EventCallback)
					data.EventCallback(event);

				break;
			}

			case WM_MOUSEWHEEL:
			{
				WindowData& data = *(WindowData*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

				SHORT scroll = (SHORT)HIWORD(wParam) / WHEEL_DELTA;
				MouseScrolledEvent event(scroll, 0.0f);
				if (data.EventCallback)
					data.EventCallback(event);

				break;
			}

			case WM_MOUSEHWHEEL:
			{
				WindowData& data = *(WindowData*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

				SHORT scroll = -(SHORT)HIWORD(wParam) / WHEEL_DELTA;
				MouseScrolledEvent event(0.0f, scroll);
				if (data.EventCallback)
					data.EventCallback(event);

				break;
			}
		}

		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}
