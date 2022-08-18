#pragma once

#include "Quark/Core/Core.h"
#include "ColorFormats.h"
#include "RenderPass.h"
#include "TextureFormats.h"

#include <vector>

namespace Quark {

	struct FramebufferAttachmentSpecification
	{
		ColorDataFormat DataFormat;
	};

	class FramebufferAttachment
	{
	public:
		FramebufferAttachment(const FramebufferAttachmentSpecification& spec);
		virtual ~FramebufferAttachment() = default;

		virtual void SetData(void* data) = 0;
		const FramebufferAttachmentSpecification& GetSpecification() const { return m_Spec; }

		static Scope<FramebufferAttachment> Create(void* image, const FramebufferAttachmentSpecification& spec);

	protected:
		FramebufferAttachmentSpecification m_Spec{};
	};

	struct FramebufferSpecification
	{
		uint32_t Width, Height;
		uint32_t Samples = 1;

		RenderPass* RenderPass;
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

		virtual bool operator==(const Framebuffer& other) const = 0;

		uint32_t GetWidth() const { return m_Spec.Width; }
		uint32_t GetHeight() const { return m_Spec.Height; }

		const FramebufferSpecification& GetSpecification() const { return m_Spec; }

		static Scope<Framebuffer> Create(const FramebufferSpecification& spec);

	protected:
		FramebufferSpecification m_Spec{};
	};
}
