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

	void OpenGLContext::CreateSwapChain(const SwapChainSpecification& spec)
	{
	}

	void OpenGLContext::SwapBuffers()
	{
		glfwSwapBuffers(m_WindowHandle);
	}

	void OpenGLContext::SetSwapInterval(int interval)
	{
		glfwSwapInterval(interval);
	}

	ViewportExtent OpenGLContext::GetViewportExtent() const
	{
		int viewportWidth, viewportHeight;
		glfwGetFramebufferSize(m_WindowHandle, &viewportWidth, &viewportHeight);
		return { (uint32_t)viewportWidth, (uint32_t)viewportHeight };
	}

	SwapSurfaceFormat OpenGLContext::ChooseSurfaceFormat(SwapSurfaceFormat preferred) const
	{
		SwapSurfaceFormat format;
		format.ColorSpace = ColorSpace::SRGBNonLinear;
		format.Format = ColorFormat::BGRA8SRGB;

		return format;
	}

	SwapPresentMode OpenGLContext::ChooseSwapPresentMode(SwapPresentMode preferred) const
	{
		return SwapPresentMode::FIFO;
	}

	SwapExtent OpenGLContext::ChooseSwapExtent(uint32_t width, uint32_t height) const
	{
		int viewportWidth, viewportHeight;
		glfwGetFramebufferSize(m_WindowHandle, &viewportWidth, &viewportHeight);
		return { (uint32_t)viewportWidth, (uint32_t)viewportHeight };
	}

	uint32_t OpenGLContext::GetSwapChainImageCount() const
	{
		return 2; // OpenGL is always double buffered
	}
}
