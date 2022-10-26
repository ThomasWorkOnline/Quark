#pragma once

#include "Quark/Renderer/GraphicsContext.h"
#include "OpenGLCommandBuffer.h"

namespace Quark {

	class OpenGLContextBase : public GraphicsContext
	{
	public:
		virtual void Init() override;
		virtual void WaitUntilDeviceIdle() final override {}

		virtual void BeginFrame(uint32_t frameIndex) final override {}
		virtual void Flush(CommandBuffer* commandBuffer, uint32_t frameIndex) final override {}

		virtual void Resize(uint32_t viewportWidth, uint32_t viewportHeight) final override {}

		virtual uint32_t GetCurrentImageIndex() const final override { return 0; }
		virtual uint32_t GetSwapChainImageCount() const final override { return 1; }

		virtual FramebufferAttachment* GetColorAttachment(uint32_t index) const final override;
	};
}
