#pragma once

#include "Quark/Core/Core.h"
#include "ColorFormats.h"
#include "RenderPass.h"
#include "TextureFormats.h"

#include <vector>

namespace Quark {

	enum class FramebufferTargetAttachment
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
		FramebufferTargetAttachment Attachment{};
		ColorDataFormat             DataFormat{};
	};

	class FramebufferAttachment
	{
	public:
		FramebufferAttachment(const FramebufferAttachmentSpecification& spec);
		virtual ~FramebufferAttachment() = default;

		virtual void SetData(void* data) = 0;
		const FramebufferAttachmentSpecification& GetSpecification() const { return m_Spec; }

		static Ref<FramebufferAttachment> Create(void* image, const FramebufferAttachmentSpecification& spec);

	protected:
		FramebufferAttachmentSpecification m_Spec;
	};

	struct FramebufferSpecification
	{
		uint32_t Width = 0, Height = 0;
		uint32_t Samples = 1;
		Ref<RenderPass> RenderPass;
		std::vector<Ref<FramebufferAttachment>> Attachments;
		bool SwapChainTarget = false;
	};

	class Framebuffer
	{
	public:
		Framebuffer(const FramebufferSpecification& spec);
		virtual ~Framebuffer() = default;

		virtual void Attach() = 0;
		virtual void Detach() = 0;

		virtual void Resize(uint32_t width, uint32_t height) = 0;

		// @Deprecated
		virtual void AttachColorTextureTarget(uint32_t target, uint32_t textureRendererID) = 0;
		virtual void AttachColorAttachment(uint32_t textureSlot, uint32_t index = 0) = 0;
		virtual void AttachDepthAttachment(uint32_t textureSlot) = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		const FramebufferSpecification& GetSpecification() const { return m_Spec; }

		virtual bool operator==(const Framebuffer& other) const = 0;

		static Ref<Framebuffer> Create(const FramebufferSpecification& spec);

	protected:
		FramebufferSpecification m_Spec;
	};
}
