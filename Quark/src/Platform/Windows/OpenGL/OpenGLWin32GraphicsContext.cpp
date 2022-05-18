#include "qkpch.h"
#include "OpenGLWin32GraphicsContext.h"

#include <glad/glad.h>
#include <Windows.h>

namespace Quark {

	OpenGLWin32GraphicsContext::OpenGLWin32GraphicsContext(void* windowHandle)
		: m_WindowHandle(static_cast<HWND>(windowHandle))
		, m_Context(nullptr)
	{
		QK_CORE_ASSERT(windowHandle, "Window handle is nullptr");
	}

	OpenGLWin32GraphicsContext::~OpenGLWin32GraphicsContext()
	{
		QK_PROFILE_FUNCTION();

		// unbind the current context
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(m_Context);
	}

	void OpenGLWin32GraphicsContext::Init()
	{
		QK_PROFILE_FUNCTION();

		PIXELFORMATDESCRIPTOR pfd = {
			sizeof(PIXELFORMATDESCRIPTOR),
			1,
			PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    // Flags
			PFD_TYPE_RGBA,        // The kind of framebuffer. RGBA or palette.
			32,                   // Colordepth of the framebuffer.
			0, 0, 0, 0, 0, 0,
			0,
			0,
			0,
			0, 0, 0, 0,
			24,                   // Number of bits for the depthbuffer
			8,                    // Number of bits for the stencilbuffer
			0,                    // Number of Aux buffers in the framebuffer.
			PFD_MAIN_PLANE,
			0,
			0, 0, 0
		};

		HDC hdc = GetDC(m_WindowHandle);
		QK_CORE_ASSERT(hdc, "Could not get a device context!");

		int format = ChoosePixelFormat(hdc, &pfd);
		SetPixelFormat(hdc, format, &pfd);

		m_Context = wglCreateContext(hdc);
		QK_CORE_ASSERT(m_Context, "Could not create a graphics context!");

		// Make the context before init OpenGL
		BOOL success = wglMakeCurrent(hdc, m_Context);
		QK_CORE_ASSERT(success, "Could not set the current context!");

		int errorCode = gladLoadGL();
		QK_CORE_ASSERT(errorCode, "Failed to initialize OpenGL context");
		QK_CORE_TRACE("Created OpenGL graphics context!");
	}

	void OpenGLWin32GraphicsContext::SwapBuffers()
	{
		::SwapBuffers(wglGetCurrentDC());
	}
}
