#pragma once

#include "Quark/Core/Core.h"
#include "CommandBuffer.h"

namespace Quark {

	class GraphicsContext : public Singleton<GraphicsContext>
	{
	public:
		GraphicsContext() : Singleton(this) {}
		virtual ~GraphicsContext() = default;

		virtual void Init() = 0;
		virtual void WaitUntilDeviceIdle() = 0;

		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;

		virtual void SwapBuffers() = 0;
		virtual void SetSwapInterval(int interval) = 0;

		virtual void Resize(uint32_t viewportWidth, uint32_t viewportHeight) = 0;

		virtual uint32_t GetCurrentImageIndex() const = 0;
		virtual uint32_t GetSwapChainImageCount() const = 0;
		virtual FramebufferAttachment* GetColorAttachment(uint32_t index) const = 0;

		virtual CommandBuffer* GetCommandBuffer() = 0;

		static Scope<GraphicsContext> Create(void* windowHandle);
	};
}
