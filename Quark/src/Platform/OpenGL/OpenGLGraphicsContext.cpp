#include "OpenGLGraphicsContext.h"

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace Quark {

	OpenGLGraphicsContext::OpenGLGraphicsContext(void* windowHandle)
		: m_WindowHandle(windowHandle)
	{
		QK_ASSERT(windowHandle, "Window handle is null");
	}

	void OpenGLGraphicsContext::Init()
	{
		QK_TIME_SCOPE_DEBUG(OpenGLGraphicsContext::Init);

		// Make the context before init glew
		glfwMakeContextCurrent(static_cast<GLFWwindow*>(m_WindowHandle));

		if (glewInit() != GLEW_OK)
			QK_FATAL("Could not init glew");
		QK_CORE_TRACE("Initialized GLEW successfully!");
	}

	void OpenGLGraphicsContext::SwapBuffers()
	{
		glfwSwapBuffers(static_cast<GLFWwindow*>(m_WindowHandle));
	}
}
