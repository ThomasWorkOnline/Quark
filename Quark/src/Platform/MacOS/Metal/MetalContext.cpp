#include "qkpch.h"
#include "MetalContext.h"

#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>
#include <GLFW/glfw3.h>

namespace Quark {

	MetalContext::MetalContext(void* windowHandle)
		: m_WindowHandle(static_cast<GLFWwindow*>(windowHandle))
	{
		QK_CORE_ASSERT(m_WindowHandle, "Window handle is nullptr");
	}

	MetalContext::~MetalContext()
	{
		QK_PROFILE_FUNCTION();

		// Detaching the current context
		glfwMakeContextCurrent(nullptr);
	}

	void MetalContext::Init()
	{
		QK_PROFILE_FUNCTION();

		// Make the context before init Metal
		glfwMakeContextCurrent(m_WindowHandle);

		// TODO(Metal): create context

		QK_CORE_TRACE("Created Metal graphics context!");
	}

	void MetalContext::SwapBuffers()
	{
		glfwSwapBuffers(m_WindowHandle);
	}
}
