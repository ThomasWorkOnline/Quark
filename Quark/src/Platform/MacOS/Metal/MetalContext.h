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

		virtual void BeginFrame() final override {}
		virtual void Flush() final override {}

		virtual void SwapBuffers() final override;
		virtual void SetSwapInterval(int interval) final override {}

		virtual void Resize(uint32_t viewportWidth, uint32_t viewportHeight) final override {}

		virtual uint32_t GetCurrentImageIndex() const final override { return 0; }
		virtual uint32_t GetSwapChainImageCount() const final override { return 1; }
		virtual FramebufferAttachment* GetColorAttachment(uint32_t index) const final override { return nullptr; }

		virtual CommandBuffer* GetCommandBuffer() final override { return nullptr; }

	private:
		GLFWwindow* m_WindowHandle;
	};
}
