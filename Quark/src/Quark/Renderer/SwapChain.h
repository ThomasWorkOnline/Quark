#pragma once

#include "Framebuffer.h"

namespace Quark {

	class SwapChain
	{
	public:
		virtual ~SwapChain() = default;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual uint32_t GetImageCount() const = 0;
		virtual uint32_t GetCurrentImageIndex() const = 0;

		virtual void AcquireNextImage() = 0;
		virtual void Present() = 0;
		virtual void Resize(uint32_t viewportWidth, uint32_t viewportHeight) = 0;
		
		virtual const Ref<FramebufferAttachment>& GetAttachment(uint32_t imageIndex) const = 0;
	};
}
