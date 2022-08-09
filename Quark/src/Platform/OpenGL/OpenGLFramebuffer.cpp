#include "qkpch.h"
#include "OpenGLFramebuffer.h"
#include "OpenGLFormats.h"

#include "Quark/Renderer/GraphicsAPI.h"

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

		static constexpr bool IsDepthAttachment(ColorDataFormat attachmentFormat)
		{
			switch (attachmentFormat)
			{
				case ColorDataFormat::Depth24:
				case ColorDataFormat::Depth24Stencil8: return true;
				default:                               return false;
			}
		}
	}

	OpenGLFramebufferAttachment::OpenGLFramebufferAttachment(void* image, const FramebufferAttachmentSpecification& spec) : FramebufferAttachment(spec)
	{
	}

	void OpenGLFramebufferAttachment::SetData(void* data)
	{
	}

	OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferSpecification& spec) : Framebuffer(spec)
	{
		QK_PROFILE_FUNCTION();

		QK_CORE_ASSERT(m_Spec.Attachments.size() <= GraphicsAPI::Instance->GetHardwareConstraints().FramebufferConstraints.MaxAttachments, "Framebuffer contains too many attachments");

		for (const auto& s : m_Spec.Attachments)
		{
			if (Utils::IsDepthAttachment(s->GetSpecification().DataFormat))
				m_DepthSpec = s->GetSpecification();
			else
				m_ColorSpecs.emplace_back(s->GetSpecification());
		}

		Invalidate();
	}

	OpenGLFramebuffer::~OpenGLFramebuffer()
	{
		QK_PROFILE_FUNCTION();

		glDeleteFramebuffers(1, &m_RendererID);
		glDeleteTextures((GLsizei)m_ColorAttachments.size(), m_ColorAttachments.data());
		glDeleteTextures(1, &m_DepthAttachment);
	}

	void OpenGLFramebuffer::Attach()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
	}

	void OpenGLFramebuffer::Detach()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFramebuffer::Resize(uint32_t width, uint32_t height)
	{
		auto& constraints = GraphicsAPI::Instance->GetHardwareConstraints();
		if (width > constraints.FramebufferConstraints.MaxWidth || height > constraints.FramebufferConstraints.MaxHeight)
		{
			QK_CORE_WARN("Attempted to resize a framebuffer with dimensions too large: {0}, {1}", width, height);
			return;
		}

		m_Spec.Width = width;
		m_Spec.Height = height;

		Invalidate();
	}

#if 0
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
#endif

	void OpenGLFramebuffer::Invalidate()
	{
		QK_PROFILE_FUNCTION();

		if (m_RendererID)
		{
			glDeleteFramebuffers(1, &m_RendererID);
			glDeleteTextures((GLsizei)m_ColorAttachments.size(), m_ColorAttachments.data());
			glDeleteTextures(1, &m_DepthAttachment);

			m_ColorAttachments.clear();
			m_DepthAttachment = 0;
		}

		glGenFramebuffers(1, &m_RendererID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
		glViewport(0, 0, m_Spec.Width, m_Spec.Height);

		// Color format
		if (m_ColorSpecs.size())
		{
			m_ColorAttachments.resize(m_ColorSpecs.size());
			glGenTextures((GLsizei)m_ColorAttachments.size(), m_ColorAttachments.data());

			for (size_t i = 0; i < m_ColorAttachments.size(); i++)
			{
				bool multisampled = m_Spec.Samples > 1;
				GLenum target = multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
				glBindTexture(target, m_ColorAttachments[i]);

				if (multisampled)
				{
					glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_Spec.Samples,
						DataFormatToOpenGLInternalFormat(m_ColorSpecs[i].DataFormat), m_Spec.Width, m_Spec.Height, GL_FALSE);
				}
				else
				{
					glTexImage2D(GL_TEXTURE_2D, 0, DataFormatToOpenGLInternalFormat(m_ColorSpecs[i].DataFormat), m_Spec.Width, m_Spec.Height, 0,
						DataFormatToOpenGLStorageFormat(m_ColorSpecs[i].DataFormat), GL_UNSIGNED_BYTE, nullptr);

					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				}

				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + (GLenum)i, target, m_ColorAttachments[i], 0);
			}
		}

		// Depth stencil format
		if (m_DepthSpec.DataFormat != ColorDataFormat::None)
		{
			glGenTextures(1, &m_DepthAttachment);

			bool multisampled = m_Spec.Samples > 1;
			GLenum target = multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
			glBindTexture(target, m_DepthAttachment);

			if (multisampled)
			{
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_Spec.Samples,
					DataFormatToOpenGLInternalFormat(m_DepthSpec.DataFormat), m_Spec.Width, m_Spec.Height, GL_FALSE);
			}
			else
			{
				glTexImage2D(GL_TEXTURE_2D, 0, DataFormatToOpenGLInternalFormat(m_DepthSpec.DataFormat), m_Spec.Width, m_Spec.Height, 0,
					DataFormatToOpenGLStorageFormat(m_DepthSpec.DataFormat), GL_UNSIGNED_BYTE, nullptr);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}

			GLenum attachmentTarget = Utils::GetDepthAttachmentTarget(m_DepthSpec.DataFormat);
			glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentTarget, target, m_DepthAttachment, 0);
		}

		if (m_ColorAttachments.size() > 1)
		{
			GLenum buffers[8] = {
				GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3,
				GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5, GL_COLOR_ATTACHMENT6, GL_COLOR_ATTACHMENT7
			};

			glDrawBuffers((GLsizei)m_ColorAttachments.size(), buffers);
		}
		else if (m_ColorAttachments.empty())
		{
			glDrawBuffer(GL_NONE);
		}

		QK_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is invalid");
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}
