#pragma once

#include "Quark/Renderer/GraphicsContext.h"

typedef struct GLFWwindow GLFWwindow;

namespace Quark {

	class MetalContext final : public GraphicsContext
	{
	public:
		MetalContext(void* windowHandle);
		virtual ~MetalContext() final override;

		virtual void Init() final override;
		virtual void WaitUntilDeviceIdle() final override {}

		virtual void BeginFrame(uint32_t frameIndex) final override {}
		virtual void Flush(CommandBuffer* commandBuffer, uint32_t frameIndex) final override {}

		virtual void SwapBuffers() final override;
		virtual void SetSwapInterval(int interval) final override {}

		virtual void Resize(uint32_t viewportWidth, uint32_t viewportHeight) final override {}

		virtual Framebuffer* GetFramebuffer() const final override { return nullptr; }
		virtual ViewportExtent GetViewportExtent() const final override { return {}; }

	private:
		GLFWwindow* m_WindowHandle;
	};
}
