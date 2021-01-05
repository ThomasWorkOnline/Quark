#pragma once

#include "../Core/Core.h"

namespace Entropy {

	class Framebuffer
	{
	public:
		virtual ~Framebuffer() = default;

		virtual void Attach() = 0;
		virtual void Detach() = 0;

		virtual void AttachColorAttachment(uint32_t textureSlot) = 0;
		virtual void AttachDepthAttachment(uint32_t textureSlot) = 0;

		virtual void Resize(uint32_t width, uint32_t height) = 0;

		virtual uint32_t GetColorAttachmentRendererID() const = 0;
		virtual uint32_t GetDepthAttachmentRendererID() const = 0;

		static Ref<Framebuffer> Create(uint32_t width, uint32_t height, uint32_t samples = 1, bool swapChainTarget = false);
	};
}
