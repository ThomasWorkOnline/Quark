#include "qkpch.h"
#include "OpenGLWin32Context.h"

#include <glad/glad.h>

namespace Quark {

	OpenGLWin32Context::OpenGLWin32Context(void* windowHandle)
		: m_WindowHandle(static_cast<HWND>(windowHandle))
		, m_DeviceContext(nullptr)
		, m_Context(nullptr)
	{
		QK_CORE_ASSERT(windowHandle, "Window handle is nullptr");
	}

	OpenGLWin32Context::~OpenGLWin32Context()
	{
		QK_PROFILE_FUNCTION();

		// unbind the current context
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(m_Context);
	}

	void OpenGLWin32Context::Init()
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
		QK_CORE_ASSERT(m_DeviceContext, "Could not get a device context!");

		int format = ChoosePixelFormat(m_DeviceContext, &pfd);
		SetPixelFormat(m_DeviceContext, format, &pfd);

		m_Context = wglCreateContext(m_DeviceContext);
		QK_CORE_ASSERT(m_Context, "Could not create a graphics context!");

		// Make the context before init OpenGL
		BOOL success = wglMakeCurrent(m_DeviceContext, m_Context);
		QK_CORE_ASSERT(success, "Could not set the current context!");

		OpenGLContextBase::Init();
	}

	void OpenGLWin32Context::StartFrame()
	{
	}

	void OpenGLWin32Context::Submit()
	{
	}

	void OpenGLWin32Context::SwapBuffers()
	{
		::SwapBuffers(m_DeviceContext);
	}

	void OpenGLWin32Context::OnViewportResized(uint32_t viewportWidth, uint32_t viewportHeight)
	{
		OpenGLContextBase::OnViewportResized(viewportWidth, viewportHeight);
	}
}
