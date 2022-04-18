#pragma once

#include "Quark/Renderer/Framebuffer.h"

namespace Quark {

	class OpenGLFramebuffer : public Framebuffer
	{
	public:
		OpenGLFramebuffer(const FramebufferSpecification& spec);
		~OpenGLFramebuffer() override;

		void Attach() override;
		void Detach() override;

		void AttachColorTextureTarget(uint32_t target, uint32_t textureRendererID) override;

		void AttachColorAttachment(uint32_t textureSlot, uint32_t index) override;
		void AttachDepthAttachment(uint32_t textureSlot) override;

		void Resize(uint32_t width, uint32_t height) override;

		uint32_t GetRendererID() const override { return m_RendererID; };

		bool operator==(const Framebuffer& other) const override
		{
			return m_RendererID == ((OpenGLFramebuffer&)other).m_RendererID;
		}

		uint32_t GetColorAttachmentRendererID(uint32_t index) const override { return m_ColorAttachments[index]; }
		uint32_t GetDepthAttachmentRendererID() const override { return m_DepthAttachment; }

		uint32_t GetWidth() const override { return m_Spec.Width; }
		uint32_t GetHeight() const override { return m_Spec.Height; }

	private:
		void Invalidate();

	private:
		uint32_t m_RendererID = 0;
		FramebufferSpecification m_Spec;

		std::vector<FramebufferAttachmentSpecification> m_ColorSpecs;
		FramebufferAttachmentSpecification m_DepthSpec;

		std::vector<uint32_t> m_ColorAttachments;
		uint32_t m_DepthAttachment = 0;
		uint32_t m_RenderBuffer = 0;
	};
}
