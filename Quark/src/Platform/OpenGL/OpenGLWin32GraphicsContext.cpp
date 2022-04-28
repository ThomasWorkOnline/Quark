#include "qkpch.h"
#include "OpenGLWin32GraphicsContext.h"

#include <glad/glad.h>
#include <Windows.h>

namespace Quark {

	OpenGLWin32GraphicsContext::OpenGLWin32GraphicsContext(void* windowHandle)
		: m_WindowHandle(static_cast<HWND>(windowHandle))
		, m_DeviceContext(nullptr)
		, m_Context(nullptr)
	{
		QK_CORE_ASSERT(windowHandle, "Window handle is nullptr");
	}

	OpenGLWin32GraphicsContext::~OpenGLWin32GraphicsContext()
	{
		// unbind the current context
		wglMakeCurrent(m_DeviceContext, NULL);

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

		m_DeviceContext = GetDC(m_WindowHandle);
		int format = ChoosePixelFormat(m_DeviceContext, &pfd);
		SetPixelFormat(m_DeviceContext, format, &pfd);

		m_Context = wglCreateContext(m_DeviceContext);

		// Make the context before init OpenGL
		BOOL success = wglMakeCurrent(m_DeviceContext, m_Context);
		QK_CORE_ASSERT(success, "Could not set the current context!");

		int errorCode = gladLoadGL();
		QK_CORE_ASSERT(errorCode == 1, "Failed to initialize OpenGL context");

		QK_CORE_TRACE("Created OpenGL graphics context!");
	}

	void OpenGLWin32GraphicsContext::SwapBuffers()
	{
		::SwapBuffers(m_DeviceContext);
	}
}
