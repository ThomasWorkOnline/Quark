#pragma once

#include "Quark/Renderer/Framebuffer.h"

namespace Quark {

	class OpenGLFramebuffer : public Framebuffer
	{
	public:
		OpenGLFramebuffer(const FramebufferSpecification& spec);
		virtual ~OpenGLFramebuffer() override;

		virtual void Attach() override;
		virtual void Detach() override;

		virtual void AttachColorAttachment(uint32_t textureSlot, uint32_t index) override;
		virtual void AttachDepthAttachment(uint32_t textureSlot) override;

		virtual void Resize(uint32_t width, uint32_t height) override;

		virtual uint32_t GetRendererID() const override { return m_RendererID; };

		virtual bool operator==(const Framebuffer& other) const override
		{
			return m_RendererID == ((OpenGLFramebuffer&)other).m_RendererID;
		}

		virtual uint32_t GetColorAttachmentRendererID(uint32_t index) const override { return m_ColorAttachments[index]; }
		virtual uint32_t GetDepthAttachmentRendererID() const override { return m_DepthAttachment; }

		virtual uint32_t GetWidth() const override { return m_Spec.Width; }
		virtual uint32_t GetHeight() const override { return m_Spec.Height; }

	private:
		void Invalidate();

	private:
		uint32_t m_RendererID = 0;
		FramebufferSpecification m_Spec;

		std::vector<FramebufferTextureSpecification> m_ColorSpecs;
		FramebufferTextureSpecification m_DepthSpec = {};

		std::vector<uint32_t> m_ColorAttachments;
		uint32_t m_DepthAttachment = 0;
	};
}
