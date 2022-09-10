#include "qkpch.h"
#include "OpenGLFramebuffer.h"
#include "OpenGLFormats.h"

#include "Quark/Renderer/Renderer.h"

#include <glad/glad.h>

namespace Quark {

	namespace Utils {

		static constexpr GLenum GetDepthAttachmentTarget(ColorDataFormat attachmentDepthFormat)
		{
			switch (attachmentDepthFormat)
			{
				case ColorDataFormat::Depth24:         return GL_DEPTH_ATTACHMENT;
				case ColorDataFormat::Depth24Stencil8: return GL_DEPTH_STENCIL_ATTACHMENT;

				QK_ASSERT_NO_DEFAULT("Invalid framebuffer depth attachment");
			}

			return GL_NONE;
		}

		static constexpr bool IsDepthOrStencilAttachment(ColorDataFormat attachmentFormat)
		{
			switch (attachmentFormat)
			{
				case ColorDataFormat::Depth24:
				case ColorDataFormat::Depth24Stencil8: return true;
				default:                               return false;
			}
		}
	}

	OpenGLFramebufferAttachment::OpenGLFramebufferAttachment(const FramebufferAttachmentSpecification& spec)
		: FramebufferAttachment(spec)
	{
		glGenTextures(1, &m_RendererID);

		bool multisampled = m_Spec.Samples > 1;
		m_Target = multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
		glBindTexture(m_Target, m_RendererID);

		if (multisampled)
		{
			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_Spec.Samples,
				DataFormatToOpenGLInternalFormat(m_Spec.DataFormat), m_Spec.Width, m_Spec.Height, GL_FALSE);
		}
		else
		{
			glTexImage2D(GL_TEXTURE_2D, 0, DataFormatToOpenGLInternalFormat(m_Spec.DataFormat), m_Spec.Width, m_Spec.Height, 0,
				DataFormatToOpenGLStorageFormat(m_Spec.DataFormat), GL_UNSIGNED_BYTE, nullptr);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}
	}

	OpenGLFramebufferAttachment::~OpenGLFramebufferAttachment()
	{
		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLFramebufferAttachment::SetData(const void* data)
	{
		QK_CORE_ASSERT(false, "TODO: invalidate framebuffer attachment");
	}

	bool OpenGLFramebufferAttachment::operator==(const FramebufferAttachment& other) const
	{
		if (auto* o = dynamic_cast<decltype(this)>(&other))
			return m_RendererID == o->m_RendererID;

		return false;
	}

	GLuint s_ActiveFramebuffer = 0;

	OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferSpecification& spec)
		: Framebuffer(spec)
	{
		QK_PROFILE_FUNCTION();

		QK_CORE_ASSERT(m_Spec.Attachments.size() <= Renderer::GetCapabilities().FramebufferCapabilities.MaxAttachments,
			"Framebuffer contains too many attachments");

		Invalidate();
	}

	OpenGLFramebuffer::~OpenGLFramebuffer()
	{
		QK_PROFILE_FUNCTION();

		glDeleteFramebuffers(1, &m_RendererID);
	}

	void OpenGLFramebuffer::Resize(uint32_t width, uint32_t height)
	{
		auto& capabilities = Renderer::GetCapabilities();
		if (width > capabilities.FramebufferCapabilities.MaxWidth || height > capabilities.FramebufferCapabilities.MaxHeight)
		{
			QK_CORE_WARN("Attempted to resize a framebuffer with dimensions too large: {0}, {1}", width, height);
			return;
		}

		m_Spec.Width = width;
		m_Spec.Height = height;

		Invalidate();
	}

	bool OpenGLFramebuffer::operator==(const Framebuffer& other) const
	{
		if (auto* o = dynamic_cast<decltype(this)>(&other))
			return m_RendererID == o->m_RendererID;

		return false;
	}

	void OpenGLFramebuffer::Bind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
		s_ActiveFramebuffer = m_RendererID;
	}

	void OpenGLFramebuffer::Unbind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		s_ActiveFramebuffer = 0;
	}

	void OpenGLFramebuffer::Bind(GLuint rendererID)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, rendererID);
		s_ActiveFramebuffer = rendererID;
	}

	void OpenGLFramebuffer::Invalidate()
	{
		QK_PROFILE_FUNCTION();

		if (m_RendererID)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glDeleteFramebuffers(1, &m_RendererID);
		}

		glGenFramebuffers(1, &m_RendererID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
		glViewport(0, 0, m_Spec.Width, m_Spec.Height);
		
		GLsizei colorAttachmentIndex = 0;
		for (auto* attachment : m_Spec.Attachments)
		{
			auto* openglAttachment = static_cast<OpenGLFramebufferAttachment*>(attachment);

			if (Utils::IsDepthOrStencilAttachment(attachment->GetSpecification().DataFormat))
			{
				// Depth stencil format
				GLenum attachmentTarget = Utils::GetDepthAttachmentTarget(attachment->GetSpecification().DataFormat);

				glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentTarget, openglAttachment->GetTarget(), openglAttachment->GetRendererID(), 0);
			}
			else
			{
				// Color format
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + colorAttachmentIndex, openglAttachment->GetTarget(), openglAttachment->GetRendererID(), 0);
				colorAttachmentIndex++;
			}
		}

		if (colorAttachmentIndex > 0)
		{
			QK_CORE_ASSERT(colorAttachmentIndex <= 8, "Max color attachment count is 8");

			GLenum buffers[8] = {
				GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3,
				GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5, GL_COLOR_ATTACHMENT6, GL_COLOR_ATTACHMENT7
			};

			glDrawBuffers(colorAttachmentIndex, buffers);
		}
		else
		{
			glDrawBuffer(GL_NONE);
		}

		QK_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is invalid");

		if (s_ActiveFramebuffer != m_RendererID)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, s_ActiveFramebuffer);
		}
	}
}
