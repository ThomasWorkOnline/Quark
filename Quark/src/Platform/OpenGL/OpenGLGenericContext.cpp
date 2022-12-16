#include "qkpch.h"
#include "OpenGLGenericContext.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#if QK_ASSERT_API_VALIDATION_ERRORS
#	define QK_OPENGL_ERROR_CALLBACK(message) QK_CORE_ASSERT(false, message)
#else
#	define QK_OPENGL_ERROR_CALLBACK(message) QK_CORE_ERROR(message)
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

	OpenGLGenericContext::OpenGLGenericContext(void* windowHandle)
		: m_WindowHandle(static_cast<GLFWwindow*>(windowHandle))
	{
		QK_PROFILE_FUNCTION();

		QK_CORE_ASSERT(m_WindowHandle, "Window handle is nullptr");

		// Make the context before init OpenGL
		glfwMakeContextCurrent(m_WindowHandle);

		int success = gladLoadGL();
		Verify(success, "Failed to initialize OpenGL context");
	}

	OpenGLGenericContext::~OpenGLGenericContext()
	{
		QK_PROFILE_FUNCTION();

		// Detaching the current context
		glfwMakeContextCurrent(NULL);
	}

	void OpenGLGenericContext::Init(const SwapChainSpecification& spec)
	{
		QK_PROFILE_FUNCTION();

		// Create swapchain with specification
		m_SwapChain = { spec };

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

	void OpenGLGenericContext::SwapBuffers()
	{
		glfwSwapBuffers(m_WindowHandle);
	}

	void OpenGLGenericContext::SetSwapInterval(int interval)
	{
		glfwSwapInterval(interval);
	}

	void OpenGLGenericContext::Resize(uint32_t viewportWidth, uint32_t viewportHeight)
	{
		m_SwapChain.Resize(viewportWidth, viewportHeight);
	}

	ViewportExtent OpenGLGenericContext::QuerySwapExtent() const
	{
		int viewportWidth, viewportHeight;
		glfwGetFramebufferSize(m_WindowHandle, &viewportWidth, &viewportHeight);
		return { (uint32_t)viewportWidth, (uint32_t)viewportHeight };
	}

	SwapSurfaceFormat OpenGLGenericContext::ChooseSurfaceFormat(SwapSurfaceFormat preferred) const
	{
		SwapSurfaceFormat format{};
		format.ColorSpace = ColorSpace::SRGBNonLinear;
		format.Format = ColorFormat::BGRA8SRGB;

		return format;
	}
}
