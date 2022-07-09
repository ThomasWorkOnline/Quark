#pragma once

#include "Quark/Renderer/Framebuffer.h"

namespace Quark {

	class OpenGLFramebuffer final : public Framebuffer
	{
	public:
		OpenGLFramebuffer(const FramebufferSpecification& spec);
		virtual ~OpenGLFramebuffer() override;

		virtual void Attach() override;
		virtual void Detach() override;

		virtual void Resize(uint32_t width, uint32_t height) override;

		virtual void AttachColorTextureTarget(uint32_t target, uint32_t textureRendererID) override;
		virtual void AttachColorAttachment(uint32_t textureSlot, uint32_t index) override;
		virtual void AttachDepthAttachment(uint32_t textureSlot) override;

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
		FramebufferSpecification m_Spec;

		std::vector<FramebufferAttachmentSpecification> m_ColorSpecs;
		FramebufferAttachmentSpecification m_DepthSpec;

		std::vector<uint32_t> m_ColorAttachments;
		uint32_t m_DepthAttachment = 0;
	};
}
