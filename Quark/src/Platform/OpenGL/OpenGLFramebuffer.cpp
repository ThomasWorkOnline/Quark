#include "OpenGLFramebuffer.h"
#include "OpenGLTextureFormats.h"

#include <glad/glad.h>

namespace Quark {

	static constexpr GLenum GetAttachmentTarget(FramebufferAttachment attachment)
	{
		switch (attachment)
		{
			case FramebufferAttachment::ColorAttachment0:       return GL_COLOR_ATTACHMENT0;
			case FramebufferAttachment::ColorAttachment1:       return GL_COLOR_ATTACHMENT1;
			case FramebufferAttachment::ColorAttachment2:       return GL_COLOR_ATTACHMENT2;
			case FramebufferAttachment::ColorAttachment3:       return GL_COLOR_ATTACHMENT3;
			case FramebufferAttachment::ColorAttachment4:       return GL_COLOR_ATTACHMENT4;
			case FramebufferAttachment::ColorAttachment5:       return GL_COLOR_ATTACHMENT5;
			case FramebufferAttachment::ColorAttachment6:       return GL_COLOR_ATTACHMENT6;
			case FramebufferAttachment::ColorAttachment7:       return GL_COLOR_ATTACHMENT7;
			case FramebufferAttachment::DepthAttachment:        return GL_DEPTH_ATTACHMENT;
			case FramebufferAttachment::DepthStencilAttachment: return GL_DEPTH_STENCIL_ATTACHMENT;
			default:
				QK_CORE_FATAL("Invalid framebuffer attachment");
				return GL_NONE;
		}
	}

	static constexpr bool IsDepthAttachment(FramebufferAttachment attachment)
	{
		switch (attachment)
		{
			case Quark::FramebufferAttachment::DepthAttachment:
			case Quark::FramebufferAttachment::DepthStencilAttachment: return true;
			default:                                                   return false;
		}
	}

	OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferSpecification& spec)
		: m_Spec(spec)
	{
		QK_SCOPE_TIMER(OpenGLFramebuffer::OpenGLFramebuffer);

		for (const auto& s : m_Spec.Attachments)
		{
			if (IsDepthAttachment(s.Attachment))
				m_DepthSpec = s;
			else
				m_ColorSpecs.emplace_back(s);
		}

		int32_t maxColorAttachments;
		glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxColorAttachments);
		QK_CORE_ASSERT(m_ColorSpecs.size() <= maxColorAttachments, "Framebuffer contains too many color attachments");

		Invalidate();
	}

	OpenGLFramebuffer::~OpenGLFramebuffer()
	{
		QK_SCOPE_TIMER(OpenGLFramebuffer::~OpenGLFramebuffer);

		glDeleteFramebuffers(1, &m_RendererID);
		glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
		glDeleteTextures(1, &m_DepthAttachment);
		glDeleteRenderbuffers(1, &m_RenderBuffer);
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

	void OpenGLFramebuffer::AttachColorTextureTarget(uint32_t target, uint32_t textureRendererID)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, target, textureRendererID, 0);
		QK_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is invalid");
	}

	void OpenGLFramebuffer::AttachColorAttachment(uint32_t textureSlot, uint32_t index)
	{
		glActiveTexture(GL_TEXTURE0 + textureSlot);
		glBindTexture(GL_TEXTURE_2D, m_ColorAttachments[index]);
	}

	void OpenGLFramebuffer::AttachDepthAttachment(uint32_t textureSlot)
	{
		glActiveTexture(GL_TEXTURE0 + textureSlot);
		glBindTexture(GL_TEXTURE_2D, m_DepthAttachment);
	}

	void OpenGLFramebuffer::Resize(uint32_t width, uint32_t height)
	{
		m_Spec.Width = width;
		m_Spec.Height = height;

		Invalidate();
	}

	void OpenGLFramebuffer::Invalidate()
	{
		QK_SCOPE_TIMER(OpenGLFramebuffer::Invalidate);

		if (m_RendererID)
		{
			glDeleteFramebuffers(1, &m_RendererID);
			glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
			glDeleteTextures(1, &m_DepthAttachment);
			glDeleteRenderbuffers(1, &m_RenderBuffer);

			m_ColorAttachments.clear();
			m_DepthAttachment = 0;
			m_RenderBuffer = 0;
		}

		glGenFramebuffers(1, &m_RendererID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

		// Color format
		if (m_ColorSpecs.size())
		{
			m_ColorAttachments.resize(m_ColorSpecs.size());
			glGenTextures(m_ColorAttachments.size(), m_ColorAttachments.data());

			for (size_t i = 0; i < m_ColorAttachments.size(); i++)
			{
				bool multisampled = m_Spec.Samples > 1;
				GLenum target = multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
				glBindTexture(target, m_ColorAttachments[i]);

				if (multisampled)
				{
					glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_Spec.Samples,
						GetTextureInternalFormat(m_ColorSpecs[i].InternalFormat), m_Spec.Width, m_Spec.Height, GL_FALSE);
				}
				else
				{
					glTexImage2D(GL_TEXTURE_2D, 0, GetTextureInternalFormat(m_ColorSpecs[i].InternalFormat), m_Spec.Width, m_Spec.Height, 0,
						GetTextureDataFormat(m_ColorSpecs[i].Format), GL_UNSIGNED_BYTE, nullptr);

					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				}

				glFramebufferTexture2D(GL_FRAMEBUFFER, GetAttachmentTarget(m_ColorSpecs[i].Attachment), target, m_ColorAttachments[i], 0);
			}
		}

		// Depth stencil format
		if (m_DepthSpec.Attachment != FramebufferAttachment::None)
		{
			glGenTextures(1, &m_DepthAttachment);

			bool multisampled = m_Spec.Samples > 1;
			GLenum target = multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
			glBindTexture(target, m_DepthAttachment);

			if (multisampled)
			{
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_Spec.Samples,
					GetTextureInternalFormat(m_DepthSpec.InternalFormat), m_Spec.Width, m_Spec.Height, GL_FALSE);
			}
			else
			{
				glTexImage2D(GL_TEXTURE_2D, 0, GetTextureInternalFormat(m_DepthSpec.InternalFormat), m_Spec.Width, m_Spec.Height, 0,
					GetTextureDataFormat(m_DepthSpec.Format), GL_UNSIGNED_BYTE, nullptr);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}

			GLenum attachmentTarget = GetAttachmentTarget(m_DepthSpec.Attachment);
			glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentTarget, target, m_DepthAttachment, 0);
		}

		if (m_ColorSpecs.empty() && m_DepthSpec.Attachment == FramebufferAttachment::None)
		{
			glGenRenderbuffers(1, &m_RenderBuffer);
			glBindRenderbuffer(GL_RENDERBUFFER, m_RenderBuffer);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, m_Spec.Width, m_Spec.Height);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_RenderBuffer);
		}

#if 0
		if (m_ColorAttachments.size() > 1)
		{
			GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
			glDrawBuffers(m_ColorAttachments.size(), buffers);
		}
		else if (m_ColorAttachments.empty())
		{
			glDrawBuffer(GL_NONE);
		}
#endif

		QK_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is invalid");
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}
