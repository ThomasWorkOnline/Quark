#pragma once

#include "../Core/Core.h"
#include "TextureFormats.h"

#include "Resource.h"

namespace Quark {

	struct FramebufferTextureSpecification
	{
		FramebufferTextureSpecification() = default;
		FramebufferTextureSpecification(TextureDataFormat textureFormat,
			TextureFilteringFormat filteringFormat = TextureFilteringFormat::Default,
			TextureTilingFormat tilingFormat = TextureTilingFormat::Default)
			: TextureFormat(textureFormat), FilteringFormat(filteringFormat), TilingFormat(tilingFormat) {}

		TextureDataFormat TextureFormat = TextureDataFormat::Default;
		TextureFilteringFormat FilteringFormat = TextureFilteringFormat::Default;
		TextureTilingFormat TilingFormat = TextureTilingFormat::Default;
	};

	struct FramebufferAttachmentSpecification
	{
		FramebufferAttachmentSpecification() = default;
		FramebufferAttachmentSpecification(std::initializer_list<FramebufferTextureSpecification> attachments)
			: Attachments(attachments) {}

		std::vector<FramebufferTextureSpecification> Attachments;
	};

	struct FramebufferSpecification
	{
		uint32_t Width = 0, Height = 0;
		uint32_t Samples = 1;
		FramebufferAttachmentSpecification Attachments;
		bool SwapChainTarget = false;
	};

	class Framebuffer : public Resource
	{
	public:
		virtual ~Framebuffer() = default;

		virtual void Attach() = 0;
		virtual void Detach() = 0;

		virtual void AttachColorAttachment(uint32_t textureSlot, uint32_t index = 0) = 0;
		virtual void AttachDepthAttachment(uint32_t textureSlot) = 0;

		virtual void Resize(uint32_t width, uint32_t height) = 0;

		virtual uint32_t GetRendererID() const = 0;

		virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const = 0;
		virtual uint32_t GetDepthAttachmentRendererID() const = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		static Ref<Framebuffer> Create(const FramebufferSpecification& spec);
	};
}
