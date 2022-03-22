#include "OpenGLGraphicsContext.h"

#include <glad/glad.h>
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
		QK_SCOPE_TIMER(OpenGLGraphicsContext::~OpenGLGraphicsContext);

		// Detaching the current context
		glfwMakeContextCurrent(nullptr);
	}

	void OpenGLGraphicsContext::Init()
	{
		QK_SCOPE_TIMER(OpenGLGraphicsContext::Init);

		// Make the context before init OpenGL
		glfwMakeContextCurrent(static_cast<GLFWwindow*>(m_WindowHandle));

		int errorCode = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		QK_ASSERT(errorCode == 1, "Failed to initialize OpenGL context");
	}

	void OpenGLGraphicsContext::SwapBuffers()
	{
		glfwSwapBuffers(static_cast<GLFWwindow*>(m_WindowHandle));
	}
}
