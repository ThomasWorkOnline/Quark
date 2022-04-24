#include "qkpch.h"
#include "OpenGLGraphicsContext.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Quark {

	OpenGLGraphicsContext::OpenGLGraphicsContext(void* windowHandle)
		: m_WindowHandle(windowHandle)
	{
		QK_CORE_ASSERT(windowHandle, "Window handle is null");
	}

	OpenGLGraphicsContext::~OpenGLGraphicsContext()
	{
		QK_PROFILE_FUNCTION();

		// Detaching the current context
		glfwMakeContextCurrent(nullptr);
	}

	void OpenGLGraphicsContext::Init()
	{
		QK_PROFILE_FUNCTION();

		// Make the context before init OpenGL
		glfwMakeContextCurrent(static_cast<GLFWwindow*>(m_WindowHandle));

		int errorCode = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		QK_CORE_ASSERT(errorCode == 1, "Failed to initialize OpenGL context");
		QK_CORE_TRACE("Created OpenGL graphics context!");
	}

	void OpenGLGraphicsContext::SwapBuffers()
	{
		glfwSwapBuffers(static_cast<GLFWwindow*>(m_WindowHandle));
	}
}
