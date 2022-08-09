#include "qkpch.h"
#include "OpenGLContext.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Quark {

	OpenGLContext::OpenGLContext(void* windowHandle)
		: m_WindowHandle(static_cast<GLFWwindow*>(windowHandle))
	{
		QK_CORE_ASSERT(windowHandle, "Window handle is nullptr");
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

	void OpenGLContext::StartFrame()
	{
		// TEMPORARY TODO: remove
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLContext::Submit()
	{
	}

	void OpenGLContext::SwapBuffers()
	{
		glfwSwapBuffers(m_WindowHandle);
	}

	void OpenGLContext::OnViewportResized(uint32_t viewportWidth, uint32_t viewportHeight)
	{
		OpenGLContextBase::OnViewportResized(viewportWidth, viewportHeight);
	}
}
