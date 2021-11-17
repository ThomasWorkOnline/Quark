#include "OpenGLGraphicsContext.h"

#include <GLFW/glfw3.h>

namespace Quark {

	OpenGLGraphicsContext::OpenGLGraphicsContext(void* windowHandle)
		: m_WindowHandle(windowHandle)
	{
		QK_ASSERT(windowHandle, "Window handle is null");
		QK_CORE_TRACE("Created OpenGL graphics context!");
	}

	OpenGLGraphicsContext::~OpenGLGraphicsContext()
	{
		QK_TIME_SCOPE_DEBUG(OpenGLGraphicsContext::~OpenGLGraphicsContext);

		// Detaching the current context
		glfwMakeContextCurrent(nullptr);
	}

	void OpenGLGraphicsContext::Init()
	{
		QK_TIME_SCOPE_DEBUG(OpenGLGraphicsContext::Init);

		// Make the context before init glew
		glfwMakeContextCurrent(static_cast<GLFWwindow*>(m_WindowHandle));
	}

	void OpenGLGraphicsContext::SwapBuffers()
	{
		glfwSwapBuffers(static_cast<GLFWwindow*>(m_WindowHandle));
	}
}
