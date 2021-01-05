#pragma once

#include "../../Entropy/Renderer/Framebuffer.h"

namespace Entropy {

	class OpenGLFramebuffer : public Framebuffer
	{
	public:
		OpenGLFramebuffer(uint32_t width, uint32_t height, uint32_t samples, bool swapChainTarget);
		virtual ~OpenGLFramebuffer();

		void Invalidate();

		virtual void Attach() override;
		virtual void Detach() override;

		virtual void AttachToTextureSlot(uint32_t textureSlot) override;

		virtual void Resize(uint32_t width, uint32_t height) override;

		virtual uint32_t GetColorAttachmentRendererID() const override { return m_ColorAttachment; }
		virtual uint32_t GetDepthAttachmentRendererID() const override { return m_DepthAttachment; }
	private:
		uint32_t m_RendererID = 0;
		uint32_t m_ColorAttachment = 0, m_DepthAttachment = 0;
		uint32_t m_Width, m_Height, m_Samples;
		bool m_SwapChainTarget;
	};
}
