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

		virtual Framebuffer* GetFramebuffer() const final override { return nullptr; }
	};
}
