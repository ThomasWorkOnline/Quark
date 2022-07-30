#include "qkpch.h"
#include "OpenGLSwapChain.h"

#include "OpenGLFramebuffer.h"

#include <GLFW/glfw3.h>

namespace Quark {

	OpenGLSwapChain::OpenGLSwapChain(GLFWwindow* windowHandle)
		: m_WindowHandle(windowHandle)
	{
		FramebufferAttachmentSpecification attachmentSpec;
		attachmentSpec.DataFormat = ColorDataFormat::BGRA8_SRGB;
		m_Attachment = CreateRef<OpenGLFramebufferAttachment>(nullptr, attachmentSpec);
	}

	OpenGLSwapChain::~OpenGLSwapChain()
	{
	}

	void OpenGLSwapChain::Present()
	{
		glfwSwapBuffers(m_WindowHandle);
	}

	void OpenGLSwapChain::Resize(uint32_t viewportWidth, uint32_t viewportHeight)
	{
		int width, height;
		glfwGetFramebufferSize(m_WindowHandle, &width, &height);

		m_Width = width;
		m_Height = height;
	}

	const Ref<FramebufferAttachment>& OpenGLSwapChain::GetAttachment(uint32_t imageIndex) const
	{
		return m_Attachment;
	}
}
