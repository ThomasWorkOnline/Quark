#include "OpenGLGraphicsContext.h"

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace Quark {

	OpenGLGraphicsContext::OpenGLGraphicsContext(GLFWwindow* windowHandle)
		: m_WindowHandle(windowHandle)
	{
		if (windowHandle == nullptr)
			QK_FATAL("Window handle is null");
	}

	void OpenGLGraphicsContext::Init()
	{
		QK_TIME_SCOPE_DEBUG(OpenGLGraphicsContext::Init);

		// Need to make the context before init glew
		glfwMakeContextCurrent(m_WindowHandle);

		if (glewInit() != GLEW_OK)
			QK_FATAL("Could not init glew");
		QK_CORE_TRACE("Initialized GLEW successfully!");
	}

	void OpenGLGraphicsContext::SwapBuffers()
	{
		glfwSwapBuffers(m_WindowHandle);
	}
}
