#pragma once

#include "Quark/Renderer/GraphicsContext.h"
#include "OpenGLCommandBuffer.h"

namespace Quark {

	class OpenGLContextBase : public GraphicsContext, OpenGLCommandBuffer
	{
	public:
		virtual void Init() override;
		virtual void StartFrame() override;

		virtual void Submit() override;
		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t viewportWidth, uint32_t viewportHeight) override;

		virtual uint32_t GetCurrentImageIndex() const override { return 0; }
		virtual uint32_t GetSwapChainImageCount() const override { return 1; }

		// We use the empty base optimization trick here since command buffers do not exist in OpenGL.
		// We return this, the GL context shares virtual functions with the command buffer object
		virtual CommandBuffer* GetCommandBuffer() override { return this; }
	};
}