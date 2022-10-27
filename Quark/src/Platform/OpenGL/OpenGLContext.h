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
		virtual void CreateSwapChain(const SwapChainSpecification& spec) final override;

		virtual void SwapBuffers() final override;
		virtual void SetSwapInterval(int interval) final override;

		virtual ViewportExtent GetViewportExtent() const final override;

		virtual SwapSurfaceFormat ChooseSurfaceFormat(SwapSurfaceFormat preferred) const final override;
		virtual SwapPresentMode ChooseSwapPresentMode(SwapPresentMode preferred) const final override;
		virtual SwapExtent ChooseSwapExtent(uint32_t width, uint32_t height) const final override;
		virtual uint32_t GetSwapChainImageCount() const final override;

	private:
		GLFWwindow* m_WindowHandle;
	};
}
