#pragma once

#include "Quark/Renderer/GraphicsContext.h"
#include "OpenGLCommandBuffer.h"

namespace Quark {

	class OpenGLContextBase : public GraphicsContext, OpenGLCommandBuffer
	{
	public:
		virtual void Init() override;
		virtual void WaitUntilDeviceIdle() final override {}

		virtual void BeginFrame() final override {}
		virtual void Flush() final override {}

		virtual void Resize(uint32_t viewportWidth, uint32_t viewportHeight) final override {}

		virtual uint32_t GetCurrentImageIndex() const final override { return 0; }
		virtual uint32_t GetSwapChainImageCount() const final override { return 1; }
		virtual FramebufferAttachment* GetColorAttachment(uint32_t index) const final override;

		// We use the empty base optimization trick here since command buffers do not exist in OpenGL.
		// We return this, the GL context shares virtual functions with the command buffer object
		virtual CommandBuffer* GetCommandBuffer() final override { return this; }
	};
}
