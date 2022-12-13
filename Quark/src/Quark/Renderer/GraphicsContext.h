#pragma once

#include "Quark/Core/Core.h"

#include "CommandBuffer.h"
#include "SwapChain.h"

namespace Quark {

	class GraphicsContext : public Singleton<GraphicsContext>
	{
	public:
		GraphicsContext() : Singleton(this) {}
		virtual ~GraphicsContext() = default;

		virtual void Init(const SwapChainSpecification& spec) = 0;
		virtual void WaitUntilDeviceIdle() = 0;

		virtual void BeginFrame(uint32_t frameIndex) = 0;
		virtual void Flush(CommandBuffer* commandBuffer) = 0;

		virtual void SwapBuffers() = 0;
		virtual void SetSwapInterval(int interval) = 0;

		virtual void Resize(uint32_t viewportWidth, uint32_t viewportHeight) = 0;

		virtual SwapChain*        GetSwapChain() = 0;
		virtual uint32_t          QuerySwapChainImageCount() const = 0;

		virtual SwapSurfaceFormat ChooseSurfaceFormat(SwapSurfaceFormat preferred) const = 0;
		virtual SwapPresentMode   ChooseSwapPresentMode(SwapPresentMode preferred) const = 0;
		virtual ViewportExtent    ChooseSwapExtent(uint32_t width, uint32_t height) const = 0;

		static Scope<GraphicsContext> Create(void* windowHandle, bool native = false);
	};
}
