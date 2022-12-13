#pragma once

#include "OpenGLContext.h"
#include "OpenGLSwapChain.h"

typedef struct GLFWwindow GLFWwindow;

namespace Quark {

	class OpenGLGenericContext final : public OpenGLContext
	{
	public:
		OpenGLGenericContext(void* windowHandle);
		virtual ~OpenGLGenericContext() final override;

		virtual void Init(const SwapChainSpecification& spec) final override;

		virtual void SwapBuffers() final override;
		virtual void SetSwapInterval(int interval) final override;

		virtual void Resize(uint32_t viewportWidth, uint32_t viewportHeight) final override;

		virtual SwapChain*        GetSwapChain() final override { return std::addressof(m_SwapChain); }
		virtual uint32_t          QuerySwapChainImageCount() const final override { return 1; }
		
		virtual SwapPresentMode   ChooseSwapPresentMode(SwapPresentMode preferred) const final override { return SwapPresentMode::FIFO; }
		virtual SwapSurfaceFormat ChooseSurfaceFormat(SwapSurfaceFormat preferred) const final override;
		virtual ViewportExtent    ChooseSwapExtent(uint32_t width, uint32_t height) const final override;

	private:
		GLFWwindow* m_WindowHandle;
		OpenGLSwapChain m_SwapChain;
	};
}
