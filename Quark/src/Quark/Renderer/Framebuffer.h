#pragma once

#include "Quark/Core/Core.h"
#include "TextureFormats.h"

#include <vector>

namespace Quark {

	enum class FramebufferAttachmentFormat
	{
		None = 0,

		// Color attachments
		ColorAttachment0,
		ColorAttachment1,
		ColorAttachment2,
		ColorAttachment3,
		ColorAttachment4,
		ColorAttachment5,
		ColorAttachment6,
		ColorAttachment7,

		// Depth and stencil attachments
		DepthAttachment,
		DepthStencilAttachment
	};

	struct FramebufferAttachmentSpecification
	{
		FramebufferAttachmentFormat Attachment{};
		TextureDataFormat           DataFormat{};
		TextureInternalFormat       InternalFormat{};
	};

	struct FramebufferSpecification
	{
		uint32_t Width = 0, Height = 0;
		uint32_t Samples = 1;
		std::vector<FramebufferAttachmentSpecification> Attachments;
		bool SwapChainTarget = false;
	};

	class Framebuffer
	{
	public:
		virtual ~Framebuffer() = default;

		virtual void Attach() = 0;
		virtual void Detach() = 0;

		virtual void Resize(uint32_t width, uint32_t height) = 0;

		virtual void AttachColorTextureTarget(uint32_t target, uint32_t textureRendererID) = 0;
		virtual void AttachColorAttachment(uint32_t textureSlot, uint32_t index = 0) = 0;
		virtual void AttachDepthAttachment(uint32_t textureSlot) = 0;

		virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const = 0;
		virtual uint32_t GetDepthAttachmentRendererID() const = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		virtual uint32_t GetRendererID() const = 0;

		virtual bool operator==(const Framebuffer& other) const = 0;

		static Ref<Framebuffer> Create(const FramebufferSpecification& spec);
	};
}
