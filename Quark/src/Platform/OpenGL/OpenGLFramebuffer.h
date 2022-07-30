#pragma once

#include "Quark/Renderer/Framebuffer.h"

namespace Quark {

	class OpenGLFramebufferAttachment final : public FramebufferAttachment
	{
	public:
		OpenGLFramebufferAttachment(void* image, const FramebufferAttachmentSpecification& spec);
		virtual ~OpenGLFramebufferAttachment() = default;

		virtual void SetData(void* data) override;
	};

	class OpenGLFramebuffer final : public Framebuffer
	{
	public:
		OpenGLFramebuffer(const FramebufferSpecification& spec);
		virtual ~OpenGLFramebuffer() override;

		virtual void Attach() override;
		virtual void Detach() override;

		virtual void Resize(uint32_t width, uint32_t height) override;

		virtual uint32_t GetWidth() const override { return m_Spec.Width; }
		virtual uint32_t GetHeight() const override { return m_Spec.Height; }

		virtual bool operator==(const Framebuffer& other) const override
		{
			return m_RendererID == ((OpenGLFramebuffer&)other).m_RendererID;
		}

	private:
		void Invalidate();

	private:
		uint32_t m_RendererID = 0;
		std::vector<FramebufferAttachmentSpecification> m_ColorSpecs;
		FramebufferAttachmentSpecification m_DepthSpec;

		std::vector<uint32_t> m_ColorAttachments;
		uint32_t m_DepthAttachment = 0;
	};
}
