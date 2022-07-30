#pragma once

#include "Quark/Renderer/GraphicsContext.h"
#include "OpenGLSwapChain.h"

typedef struct GLFWwindow GLFWwindow;

namespace Quark {

	class OpenGLContext final : public GraphicsContext
	{
	public:
		OpenGLContext(void* windowHandle);
		virtual ~OpenGLContext() override;

		virtual void Init() override;
		virtual void SwapBuffers() override;
		virtual SwapChain* GetSwapChain() override { return &m_SwapChain; }

	private:
		GLFWwindow* m_WindowHandle;
		OpenGLSwapChain m_SwapChain;
	};
}
