#pragma once

#include "OpenGLContextBase.h"

typedef struct GLFWwindow GLFWwindow;

namespace Quark {

	class OpenGLContext final : public OpenGLContextBase
	{
	public:
		OpenGLContext(void* windowHandle);
		virtual ~OpenGLContext() final override;

		virtual void Init() final override;
		virtual void CreateSwapChain(const RenderPass* renderPass) final override;

		virtual void SwapBuffers() final override;
		virtual void SetSwapInterval(int interval) final override;

		virtual ViewportExtent GetViewportExtent() const final override;

	private:
		GLFWwindow* m_WindowHandle;
	};
}
