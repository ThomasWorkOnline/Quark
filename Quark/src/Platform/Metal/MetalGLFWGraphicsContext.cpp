#include "qkpch.h"
#include "MetalGLFWGraphicsContext.h"

#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>
#include <GLFW/glfw3.h>

namespace Quark {

	MetalGLFWGraphicsContext::MetalGLFWGraphicsContext(void* windowHandle)
    	: m_WindowHandle(static_cast<GLFWwindow*>(windowHandle))
	{
		QK_CORE_ASSERT(windowHandle, "Window handle is nullptr");
	}

	MetalGLFWGraphicsContext::~MetalGLFWGraphicsContext()
	{
		QK_PROFILE_FUNCTION();

		// Detaching the current context
		glfwMakeContextCurrent(nullptr);
	}

	void MetalGLFWGraphicsContext::Init()
	{
		QK_PROFILE_FUNCTION();

		// Make the context before init Metal
		glfwMakeContextCurrent(m_WindowHandle);

		// TODO(Metal): create context
		
		QK_CORE_TRACE("Created Metal graphics context!");
	}

    void MetalGLFWGraphicsContext::SwapBuffers()
	{
		glfwSwapBuffers(m_WindowHandle);
	}
}
