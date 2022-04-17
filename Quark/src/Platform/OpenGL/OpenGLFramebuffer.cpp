#include "OpenGLFramebuffer.h"
#include "OpenGLTextureFormats.h"

#include <glad/glad.h>

namespace Quark {

	static constexpr GLenum GetAttachmentTarget(FramebufferAttachment attachment)
	{
		switch (attachment)
		{
			case Quark::FramebufferAttachment::ColorAttachment0:       return GL_COLOR_ATTACHMENT0;
			case Quark::FramebufferAttachment::ColorAttachment1:       return GL_COLOR_ATTACHMENT1;
			case Quark::FramebufferAttachment::ColorAttachment2:       return GL_COLOR_ATTACHMENT2;
			case Quark::FramebufferAttachment::ColorAttachment3:       return GL_COLOR_ATTACHMENT3;
			case Quark::FramebufferAttachment::DepthAttachment:        return GL_DEPTH_ATTACHMENT;
			case Quark::FramebufferAttachment::DepthStencilAttachment: return GL_DEPTH_STENCIL_ATTACHMENT;
			default:
				QK_CORE_FATAL("Invalid framebuffer attachment");
				return GL_NONE;
		}
	}

	OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferSpecification& spec)
		: m_Spec(spec)
	{
		for (const auto& s : m_Spec.Attachments)
		{
			if (IsTextureDepthFormat(s.InternalFormat))
				m_DepthSpec = s;
			else
				m_ColorSpecs.emplace_back(s);
		}

		Invalidate();
	}

	OpenGLFramebuffer::~OpenGLFramebuffer()
	{
		glDeleteFramebuffers(1, &m_RendererID);
		glDeleteRenderbuffers(m_ColorAttachments.size(), m_ColorAttachments.data());
		glDeleteRenderbuffers(1, &m_DepthAttachment);
	}

	void OpenGLFramebuffer::Attach()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
		glViewport(0, 0, m_Spec.Width, m_Spec.Height);
	}

	void OpenGLFramebuffer::Detach()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFramebuffer::AttachColorAttachment(uint32_t textureSlot, uint32_t index)
	{
		glActiveTexture(GL_TEXTURE0 + textureSlot);
		glBindRenderbuffer(GL_RENDERBUFFER, m_ColorAttachments[index]);
	}

	void OpenGLFramebuffer::AttachDepthAttachment(uint32_t textureSlot)
	{
		glActiveTexture(GL_TEXTURE0 + textureSlot);
		glBindRenderbuffer(GL_RENDERBUFFER, m_DepthAttachment);
	}

	void OpenGLFramebuffer::Resize(uint32_t width, uint32_t height)
	{
		m_Spec.Width = width;
		m_Spec.Height = height;

		Invalidate();
	}

	void OpenGLFramebuffer::Invalidate()
	{
		if (m_RendererID)
		{
			glDeleteFramebuffers(1, &m_RendererID);
			glDeleteRenderbuffers(m_ColorAttachments.size(), m_ColorAttachments.data());
			glDeleteRenderbuffers(1, &m_DepthAttachment);

			m_ColorAttachments.clear();
			m_DepthAttachment = 0;
		}

		glGenFramebuffers(1, &m_RendererID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

		// Color format
		if (m_ColorSpecs.size())
		{
			m_ColorAttachments.resize(m_ColorSpecs.size());
			glGenRenderbuffers(m_ColorAttachments.size(), m_ColorAttachments.data());

			for (size_t i = 0; i < m_ColorAttachments.size(); i++)
			{
				bool multisampled = m_Spec.Samples > 1;
				if (multisampled)
				{
					glBindRenderbuffer(GL_RENDERBUFFER, m_ColorAttachments[i]);
					glRenderbufferStorageMultisample(GL_RENDERBUFFER, m_Spec.Samples, GetTextureInternalFormat(m_ColorSpecs[i].InternalFormat), m_Spec.Width, m_Spec.Height);
				}
				else
				{
					glBindRenderbuffer(GL_RENDERBUFFER, m_ColorAttachments[i]);
					glRenderbufferStorage(GL_RENDERBUFFER, GetTextureInternalFormat(m_ColorSpecs[i].InternalFormat), m_Spec.Width, m_Spec.Height);
				}

				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_RENDERBUFFER, m_ColorAttachments[i]);
			}
		}

		// Depth stencil format
		if (m_DepthSpec.Attachment != FramebufferAttachment::None)
		{
			glGenRenderbuffers(1, &m_DepthAttachment);

			bool multisampled = m_Spec.Samples > 1;
			if (multisampled)
			{
				glBindRenderbuffer(GL_RENDERBUFFER, m_DepthAttachment);
				glRenderbufferStorageMultisample(GL_RENDERBUFFER, m_Spec.Samples, GetTextureInternalFormat(m_DepthSpec.InternalFormat), m_Spec.Width, m_Spec.Height);
			}
			else
			{
				glBindRenderbuffer(GL_RENDERBUFFER, m_DepthAttachment);
				glRenderbufferStorage(GL_RENDERBUFFER, GetTextureInternalFormat(m_DepthSpec.InternalFormat), m_Spec.Width, m_Spec.Height);
			}

			GLenum target = GetAttachmentTarget(m_DepthSpec.Attachment);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, target, GL_RENDERBUFFER, m_DepthAttachment);
		}

		if (m_ColorAttachments.size() > 1)
		{
			QK_CORE_ASSERT(m_ColorAttachments.size() <= 4, "Framebuffer contains too many color attachments (maximum is 4)");
			GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
			glDrawBuffers(m_ColorAttachments.size(), buffers);
		}
		else if (m_ColorAttachments.empty())
		{
			glDrawBuffer(GL_NONE);
		}

		QK_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is invalid");
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}
