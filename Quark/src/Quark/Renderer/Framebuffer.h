#pragma once

#include "Quark/Core/Core.h"
#include "ColorFormats.h"
#include "RenderPass.h"
#include "TextureFormats.h"

#include <vector>

namespace Quark {

	struct FramebufferAttachmentSpecification
	{
		uint32_t Width = 0, Height = 0;
		uint32_t Samples = 1;
		ColorDataFormat DataFormat{};
	};

	class FramebufferAttachment
	{
	public:
		FramebufferAttachment() = default;
		FramebufferAttachment(const FramebufferAttachmentSpecification& spec);
		virtual ~FramebufferAttachment() = default;

		virtual void SetData(const void* data) = 0;

		virtual bool operator==(const FramebufferAttachment& other) const = 0;

		const FramebufferAttachmentSpecification& GetSpecification() const { return m_Spec; }

		static Scope<FramebufferAttachment> Create(const FramebufferAttachmentSpecification& spec);

	protected:
		FramebufferAttachmentSpecification m_Spec{};
	};

	struct FramebufferSpecification
	{
		uint32_t Width = 0, Height = 0;
		uint32_t Samples = 1;

		RenderPass* RenderPass = nullptr;
		bool SwapChainTarget = false;

		std::vector<FramebufferAttachmentSpecification> Specifications;
	};

	class Framebuffer
	{
	public:
		Framebuffer(const FramebufferSpecification& spec);
		virtual ~Framebuffer() = default;

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
