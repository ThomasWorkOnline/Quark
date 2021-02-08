#include "OpenGLFramebuffer.h"

#include <GL/glew.h>

namespace Entropy {

	static inline GLenum GetTextureSampleMode(bool multisampled)
	{
		return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
	}

	static inline GLenum GetTextureInternalFormat(FramebufferTextureFormat format)
	{
		switch (format)
		{
		case FramebufferTextureFormat::RGBA8:           return GL_RGBA8;
		case FramebufferTextureFormat::Depth24Stencil8: return GL_DEPTH24_STENCIL8;
		}

		NT_FATAL("Invalid framebuffer internal texture format");
	}

	static inline GLenum GetTextureColorFormat(FramebufferTextureFormat format)
	{
		switch (format)
		{
		case FramebufferTextureFormat::RGBA8: return GL_RGBA;
		}

		NT_FATAL("Invalid framebuffer texture color format");
	}

	static inline GLenum GetFilteringFormat(FramebufferFilteringFormat format)
	{
		switch (format)
		{
		case FramebufferFilteringFormat::Nearest: return GL_NEAREST;
		case FramebufferFilteringFormat::Linear:  return GL_LINEAR;
		}

		NT_FATAL("Invalid framebuffer filtering format");
	}

	static inline GLenum GetTilingFormat(FramebufferTilingFormat format)
	{
		switch (format)
		{
		case FramebufferTilingFormat::Clamp:  return GL_CLAMP_TO_EDGE;
		case FramebufferTilingFormat::Repeat: return GL_REPEAT;
		}

		NT_FATAL("Invalid framebuffer tiling format");
	}

	static inline bool IsDepthFormat(FramebufferTextureFormat format)
	{
		switch (format)
		{
		case FramebufferTextureFormat::Depth24Stencil8: return true;
		}
		return false;
	}

	OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferSpecification& spec)
		: m_Spec(spec)
	{
		for (auto s : m_Spec.Attachments.Attachments)
		{
			if (!IsDepthFormat(s.TextureFormat))
				m_ColorSpecs.emplace_back(s);
			else
				m_DepthSpec = s;
		}

		Invalidate();
	}

	OpenGLFramebuffer::~OpenGLFramebuffer()
	{
		glDeleteFramebuffers(1, &m_RendererID);
		glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
		glDeleteTextures(1, &m_DepthAttachment);
	}

	void OpenGLFramebuffer::Invalidate()
	{
		if (m_RendererID)
		{
			glDeleteFramebuffers(1, &m_RendererID);
			glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
			glDeleteTextures(1, &m_DepthAttachment);

			m_ColorAttachments.clear();
			m_DepthAttachment = 0;
		}

		glCreateFramebuffers(1, &m_RendererID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

		bool multisampled = m_Spec.Samples > 1;

		// Color format
		if (m_ColorSpecs.size())
		{
			m_ColorAttachments.resize(m_ColorSpecs.size());
			glCreateTextures(GetTextureSampleMode(multisampled), m_ColorAttachments.size(), m_ColorAttachments.data());

			for (size_t i = 0; i < m_ColorAttachments.size(); i++)
			{
				glBindTexture(GetTextureSampleMode(multisampled), m_ColorAttachments[i]);

				if (multisampled)
				{
					glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_Spec.Samples, GetTextureInternalFormat(m_ColorSpecs[i].TextureFormat), m_Spec.Width, m_Spec.Height, GL_FALSE);
				}
				else
				{
					glTexImage2D(GL_TEXTURE_2D, 0, GetTextureInternalFormat(m_ColorSpecs[i].TextureFormat), m_Spec.Width, m_Spec.Height, 0,
						GetTextureColorFormat(m_ColorSpecs[i].TextureFormat), GL_UNSIGNED_BYTE, nullptr);

					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GetFilteringFormat(m_ColorSpecs[i].FilteringFormat));
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GetFilteringFormat(m_ColorSpecs[i].FilteringFormat));
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GetTilingFormat(m_ColorSpecs[i].TilingFormat));
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GetTilingFormat(m_ColorSpecs[i].TilingFormat));
				}

				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GetTextureSampleMode(multisampled), m_ColorAttachments[i], 0);
			}
		}

		// Depth stencil format
		if (m_DepthSpec.TextureFormat != FramebufferTextureFormat::None)
		{
			glCreateTextures(GetTextureSampleMode(multisampled), 1, &m_DepthAttachment);
			glBindTexture(GetTextureSampleMode(multisampled), m_DepthAttachment);

			if (multisampled)
			{
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_Spec.Samples, GetTextureInternalFormat(m_DepthSpec.TextureFormat), m_Spec.Width, m_Spec.Height, GL_FALSE);
			}
			else
			{
				glTexStorage2D(GL_TEXTURE_2D, 1, GetTextureInternalFormat(m_DepthSpec.TextureFormat), m_Spec.Width, m_Spec.Height);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GetFilteringFormat(m_DepthSpec.FilteringFormat));
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GetFilteringFormat(m_DepthSpec.FilteringFormat));
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GetTilingFormat(m_DepthSpec.TilingFormat));
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GetTilingFormat(m_DepthSpec.TilingFormat));
			}

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GetTextureSampleMode(multisampled), m_DepthAttachment, 0);
		}

		if (m_ColorAttachments.size() > 1)
		{
			NT_ASSERT(m_ColorAttachments.size() <= 4, "Framebuffer contains too many color attachments (maximum is 4)");
			GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
			glDrawBuffers(m_ColorAttachments.size(), buffers);
		}
		else if (m_ColorAttachments.empty())
		{
			glDrawBuffer(GL_NONE);
		}

		NT_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is invalid");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFramebuffer::Attach()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
		// TODO: make the viewport part of the render pipeline
		glViewport(0, 0, m_Spec.Width, m_Spec.Height);
	}

	void OpenGLFramebuffer::Detach()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
}
