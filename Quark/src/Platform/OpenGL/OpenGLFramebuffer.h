#pragma once

#include "Quark/Renderer/Framebuffer.h"

typedef unsigned int GLuint;
typedef unsigned int GLenum;

namespace Quark {

	class OpenGLFramebufferAttachment final : public FramebufferAttachment
	{
	public:
		OpenGLFramebufferAttachment() = default;
		OpenGLFramebufferAttachment(const FramebufferAttachmentSpecification& spec);
		virtual ~OpenGLFramebufferAttachment() final override;

		virtual void SetData(const void* data) final override;

		virtual bool operator==(const FramebufferAttachment& other) const final override
		{
			return m_RendererID == reinterpret_cast<decltype(this)>(&other)->m_RendererID;
		}

		// Non-Copyable
		OpenGLFramebufferAttachment(const OpenGLFramebufferAttachment&) = delete;
		OpenGLFramebufferAttachment& operator=(const OpenGLFramebufferAttachment&) = delete;

	private:
		GLuint m_RendererID = 0;
		GLenum m_Target = 0;

		friend class OpenGLFramebuffer;
	};

	class OpenGLFramebuffer final : public Framebuffer
	{
	public:
		OpenGLFramebuffer(const FramebufferSpecification& spec);
		virtual ~OpenGLFramebuffer() final override;

		virtual void Resize(uint32_t width, uint32_t height) final override;

		virtual bool operator==(const Framebuffer& other) const final override
		{
			return m_RendererID == reinterpret_cast<decltype(this)>(&other)->m_RendererID;
		}

		// Non-Copyable
		OpenGLFramebuffer(const OpenGLFramebuffer&) = delete;
		OpenGLFramebuffer& operator=(const OpenGLFramebuffer&) = delete;

		void Bind();
		void Unbind();

		static void Bind(GLuint rendererID);

	private:
		void Invalidate();

	private:
		GLuint m_RendererID = 0;
		Array<OpenGLFramebufferAttachment> m_ColorAttachments;
	};
}
