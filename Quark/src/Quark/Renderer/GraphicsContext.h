#pragma once

#include "Quark/Core/Core.h"
#include "Quark/Core/Window.h"

#include "CommandBuffer.h"

namespace Quark {

	class GraphicsContext : public Singleton<GraphicsContext>
	{
	public:
		GraphicsContext() : Singleton(this) {}
		virtual ~GraphicsContext() = default;

		virtual void Init() = 0;
		virtual void WaitUntilDeviceIdle() = 0;

		virtual void BeginFrame(uint32_t frameIndex) = 0;
		virtual void Flush(CommandBuffer* commandBuffer, uint32_t frameIndex) = 0;

		virtual void SwapBuffers() = 0;
		virtual void SetSwapInterval(int interval) = 0;

		virtual void Resize(uint32_t viewportWidth, uint32_t viewportHeight) = 0;

		virtual uint32_t GetCurrentImageIndex() const = 0;
		virtual uint32_t GetSwapChainImageCount() const = 0;

		virtual FramebufferAttachment* GetColorAttachment(uint32_t index) const = 0;

		static Scope<GraphicsContext> Create(void* windowHandle, bool native = false);
	};
}
