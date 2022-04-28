#include "qkpch.h"
#include "OpenGLGLFWGraphicsContext.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Quark {

	OpenGLGLFWGraphicsContext::OpenGLGLFWGraphicsContext(void* windowHandle)
		: m_WindowHandle(static_cast<GLFWwindow*>(windowHandle))
	{
		QK_CORE_ASSERT(windowHandle, "Window handle is nullptr");
	}

	OpenGLGLFWGraphicsContext::~OpenGLGLFWGraphicsContext()
	{
		QK_PROFILE_FUNCTION();

		// Detaching the current context
		glfwMakeContextCurrent(nullptr);
	}

	void OpenGLGLFWGraphicsContext::Init()
	{
		QK_PROFILE_FUNCTION();

		// Make the context before init OpenGL
		glfwMakeContextCurrent(m_WindowHandle);

		int errorCode = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		QK_CORE_ASSERT(errorCode == 1, "Failed to initialize OpenGL context");
		QK_CORE_TRACE("Created OpenGL graphics context!");
	}

	void OpenGLGLFWGraphicsContext::SwapBuffers()
	{
		glfwSwapBuffers(m_WindowHandle);
	}
}
