#include "qkpch.h"
#include "OpenGLContext.h"

#include <GLFW/glfw3.h>

namespace Quark {

	OpenGLContext::OpenGLContext(void* windowHandle)
		: m_WindowHandle(static_cast<GLFWwindow*>(windowHandle))
	{
		QK_CORE_ASSERT(m_WindowHandle, "Window handle is nullptr");
	}

	OpenGLContext::~OpenGLContext()
	{
		QK_PROFILE_FUNCTION();

		// Detaching the current context
		glfwMakeContextCurrent(nullptr);
	}

	void OpenGLContext::Init()
	{
		QK_PROFILE_FUNCTION();

		// Make the context before init OpenGL
		glfwMakeContextCurrent(m_WindowHandle);

		OpenGLContextBase::Init();
	}

	void OpenGLContext::SwapBuffers()
	{
		glfwSwapBuffers(m_WindowHandle);
	}

	void OpenGLContext::SetSwapInterval(int interval)
	{
		glfwSwapInterval(interval);
	}
}
