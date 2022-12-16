#pragma once

#include "Quark/Renderer/GraphicsContext.h"

typedef struct GLFWwindow GLFWwindow;

namespace Quark {

	class MetalContext final : public GraphicsContext
	{
	public:
		MetalContext(void* windowHandle);
		virtual ~MetalContext() final override;

		virtual void Init(const SwapChainSpecification& spec) final override;
		virtual void WaitUntilDeviceIdle() final override {}

		virtual void BeginFrame(uint32_t frameIndex) final override {}
		virtual void Flush(CommandBuffer* commandBuffer) final override {}

		virtual void SwapBuffers() final override;
		virtual void SetSwapInterval(int interval) final override {}

		virtual void Resize(uint32_t viewportWidth, uint32_t viewportHeight) final override {}

		virtual SwapChain*        GetSwapChain() final override { return nullptr; }
		virtual uint32_t          QuerySwapChainImageCount() const final override { return 0; }
		virtual ViewportExtent    QuerySwapExtent() const final override { return {}; }

		virtual SwapSurfaceFormat ChooseSurfaceFormat(SwapSurfaceFormat preferred) const final override { return {}; }
		virtual SwapPresentMode   ChooseSwapPresentMode(SwapPresentMode preferred) const final override { return {}; }

	private:
		GLFWwindow* m_WindowHandle;
	};
}
