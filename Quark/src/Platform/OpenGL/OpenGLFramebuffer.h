#pragma once

#include "Quark/Renderer/Framebuffer.h"

typedef unsigned int GLuint;

namespace Quark {

	class OpenGLFramebufferAttachment final : public FramebufferAttachment
	{
	public:
		OpenGLFramebufferAttachment(const FramebufferAttachmentSpecification& spec);
		virtual ~OpenGLFramebufferAttachment() = default;

		virtual void SetData(void* data) override;

		// Non-Copyable
		OpenGLFramebufferAttachment(const OpenGLFramebufferAttachment&) = delete;
		OpenGLFramebufferAttachment& operator=(const OpenGLFramebufferAttachment&) = delete;
	};

	class OpenGLFramebuffer final : public Framebuffer
	{
	public:
		OpenGLFramebuffer(const FramebufferSpecification& spec);
		virtual ~OpenGLFramebuffer() override;

		virtual void Attach() override;
		virtual void Detach() override;

		virtual void Resize(uint32_t width, uint32_t height) override;

		virtual bool operator==(const Framebuffer& other) const override
		{
			return m_RendererID == reinterpret_cast<decltype(*this)&>(other).m_RendererID;
		}

		// Non-Copyable
		OpenGLFramebuffer(const OpenGLFramebuffer&) = delete;
		OpenGLFramebuffer& operator=(const OpenGLFramebuffer&) = delete;

	private:
		void Invalidate();

	private:
		GLuint m_RendererID = 0;
		std::vector<FramebufferAttachmentSpecification> m_ColorSpecs;
		FramebufferAttachmentSpecification m_DepthSpec;

		std::vector<uint32_t> m_ColorAttachments;
		uint32_t m_DepthAttachment = 0;
	};
}
