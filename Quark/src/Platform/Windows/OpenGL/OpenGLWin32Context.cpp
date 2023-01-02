#include "qkpch.h"
#include "OpenGLWin32Context.h"

#include <Windows.h>
#include <glad/glad.h>

#if QK_ASSERT_API_VALIDATION_ERRORS
	#define QK_OPENGL_ERROR_CALLBACK(message) QK_CORE_ASSERT(false, message)
#else
	#define QK_OPENGL_ERROR_CALLBACK(message) QK_CORE_ERROR(message)
#endif

namespace Quark {

	static void OnOpenGLMessage(
		GLenum source,
		GLenum type,
		GLuint id,
		GLenum severity,
		GLsizei length,
		const GLchar* message,
		const void* userParam)
	{
		switch (severity)
		{
			case GL_DEBUG_SEVERITY_HIGH:         QK_OPENGL_ERROR_CALLBACK(message); return;
			case GL_DEBUG_SEVERITY_MEDIUM:       QK_CORE_WARN(message);             return;
			case GL_DEBUG_SEVERITY_LOW:          QK_CORE_INFO(message);             return;
			case GL_DEBUG_SEVERITY_NOTIFICATION: QK_CORE_TRACE(message);            return;

			QK_ASSERT_NO_DEFAULT("OnOpenGLMessage had an unknown severity level");
		}
	}

	OpenGLWin32Context::OpenGLWin32Context(void* windowHandle)
		: m_WindowHandle(static_cast<HWND>(windowHandle))
		, m_DeviceContext(nullptr)
		, m_Context(nullptr)
	{
		QK_PROFILE_FUNCTION();

		QK_CORE_ASSERT(m_WindowHandle, "Window handle is nullptr");

		// Initialize WGL
		HDC dc = GetDC(m_WindowHandle);
		QK_CORE_ASSERT(dc, "Could not get a device context!");

		PIXELFORMATDESCRIPTOR pfd{};
		pfd.nSize = sizeof(pfd);
		pfd.nVersion = 1;
		pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
		pfd.iPixelType = PFD_TYPE_RGBA;
		pfd.cColorBits = 24;

		int format = ChoosePixelFormat(dc, &pfd);
		SetPixelFormat(dc, format, &pfd);

		HGLRC rc = wglCreateContext(dc);
		QK_CORE_ASSERT(rc, "Could not create a graphics context!");

		m_DeviceContext = wglGetCurrentDC();
		m_Context = wglGetCurrentContext();

		if (!wglMakeCurrent(dc, rc))
		{
			wglMakeCurrent(m_DeviceContext, m_Context);
			wglDeleteContext(rc);

			ThrowRuntimeError("Could not set the current context!");
		}

		int success = gladLoadGL();
		Verify(success, "Failed to initialize OpenGL context");

		wglMakeCurrent(m_DeviceContext, m_Context);
		wglDeleteContext(rc);
	}

	OpenGLWin32Context::~OpenGLWin32Context()
	{
		QK_PROFILE_FUNCTION();

		// Unbind the current context
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(m_Context);
	}

	void OpenGLWin32Context::Init(const SwapChainSpecification& spec)
	{
		QK_PROFILE_FUNCTION();

		// TODO: support multisampling
		//       support vsync
		//       resizing

		// Create WGL context
		m_DeviceContext = GetDC(m_WindowHandle);
		QK_CORE_ASSERT(m_DeviceContext, "Could not get a device context!");

		// Create swapchain with specification
		m_SwapChain = { spec };

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

		int format = ChoosePixelFormat(m_DeviceContext, &pfd);
		SetPixelFormat(m_DeviceContext, format, &pfd);

		m_Context = wglCreateContext(m_DeviceContext);
		QK_CORE_ASSERT(m_Context, "Could not create a graphics context!");

		BOOL success = wglMakeCurrent(m_DeviceContext, m_Context);
		QK_CORE_ASSERT(success, "Could not set the current context!");

#ifdef QK_DEBUG
		if (glDebugMessageCallback)
		{
			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

			glDebugMessageCallback(OnOpenGLMessage, nullptr); // <-- This is not supported on OpenGL 4.2 or lower
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
		}
#endif

		// Gamma correction
		glEnable(GL_FRAMEBUFFER_SRGB);

		// Alpha and Blending
		glEnable(GL_BLEND);

		// Face Culling
		glEnable(GL_CULL_FACE);

		// Depth Testing
		glEnable(GL_DEPTH_TEST);

		glDepthRangef(0.0f, 1.0f);

		glFrontFace(GL_CCW);
		//           ^^^-- we use a counter-clockwise winding order

		// Filtering
		glEnable(GL_MULTISAMPLE);
		glEnable(GL_LINE_SMOOTH);

		// Viewport in the same range as Vulkan and DirectX
		//glClipControl(GL_UPPER_LEFT, GL_ZERO_TO_ONE);

		// Experimental
		glEnable(GL_PROGRAM_POINT_SIZE);

		QK_CORE_TRACE("Created OpenGL graphics context!");
	}

	void OpenGLWin32Context::SwapBuffers()
	{
		::SwapBuffers(m_DeviceContext);
	}

	void OpenGLWin32Context::SetSwapInterval(int interval)
	{
	}

	void OpenGLWin32Context::Resize(uint32_t viewportWidth, uint32_t viewportHeight)
	{
		m_SwapChain.Resize(viewportWidth, viewportHeight);
	}

	ViewportExtent OpenGLWin32Context::QuerySwapExtent() const
	{
		RECT extent{};
		GetClientRect(m_WindowHandle, &extent);

		uint32_t w = extent.right - extent.left;
		uint32_t h = extent.bottom - extent.top;

		return { w, h };
	}

	SwapSurfaceFormat OpenGLWin32Context::ChooseSurfaceFormat(SwapSurfaceFormat preferred) const
	{
		SwapSurfaceFormat format{};
		format.ColorSpace = ColorSpace::SRGBNonLinear;
		format.Format = ColorFormat::BGRA8SRGB;

		return format;
	}
}
