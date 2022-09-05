#include "qkpch.h"
#include "OpenGLTexture.h"
#include "OpenGLFormats.h"

#include "Quark/Renderer/Image.h"
#include "Quark/Renderer/Renderer.h"

#include <glad/glad.h>

namespace Quark {

	OpenGLTexture2D::OpenGLTexture2D(const Texture2DSpecification& spec)
		: Texture2D(spec)
	{
		QK_PROFILE_FUNCTION();

		QK_CORE_ASSERT(!IsformatUsingMips(spec.RenderModes.MagFilteringMode),
			"The magnification mode may not be set to use mipmaps");

		QK_CORE_ASSERT(m_Spec.Width <= Renderer::GetCapabilities().TextureCapabilities.MaxPixelSize
			&& m_Spec.Height <= Renderer::GetCapabilities().TextureCapabilities.MaxPixelSize, "Texture dimensions too large");

		m_InternalFormat = DataFormatToOpenGLInternalFormat(m_Spec.DataFormat);
		m_DataFormat = DataFormatToOpenGLStorageFormat(m_Spec.DataFormat);
		QK_CORE_ASSERT(m_InternalFormat & m_DataFormat, "Image format not supported");

		glGenTextures(1, &m_RendererID);

		bool multisampled = m_Spec.Samples > 1;
		if (multisampled)
		{
			QK_CORE_ASSERT(m_Spec.Levels == 0, "Mipmap level must be set to 0 when multisampling is enabled");

			glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_RendererID);
			glPixelStorei(GL_UNPACK_ALIGNMENT, IsPixel4BytesAligned(m_Spec.DataFormat) ? 4 : 1);
			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_Spec.Samples, m_InternalFormat, m_Spec.Width, m_Spec.Height, GL_FALSE);

			QK_DEBUG_CALL(glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0));
		}
		else
		{
			if (m_Spec.Levels > 0)
			{
				QK_CORE_ASSERT(IsformatUsingMips(m_Spec.RenderModes.MinFilteringMode),
					"Mipmap level must match the texture minification mode");
			}

			glBindTexture(GL_TEXTURE_2D, m_RendererID);
			glPixelStorei(GL_UNPACK_ALIGNMENT, IsPixel4BytesAligned(m_Spec.DataFormat) ? 4 : 1);
			glTexImage2D(GL_TEXTURE_2D, m_Spec.Levels, m_InternalFormat, m_Spec.Width, m_Spec.Height, 0, m_DataFormat,
				DataFormatToOpenGLDataType(m_Spec.DataFormat), nullptr);

			GLenum tilingMode = TilingModeToOpenGL(m_Spec.RenderModes.TilingMode);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, FilteringModeToOpenGL(m_Spec.RenderModes.MinFilteringMode));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, FilteringModeToOpenGL(m_Spec.RenderModes.MagFilteringMode));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, tilingMode);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tilingMode);

			QK_DEBUG_CALL(glBindTexture(GL_TEXTURE_2D, 0));
		}
	}

	OpenGLTexture2D::OpenGLTexture2D(std::string_view filepath, const TextureRenderModes& renderModes)
	{
		QK_PROFILE_FUNCTION();

		QK_CORE_ASSERT(!IsformatUsingMips(renderModes.MagFilteringMode),
			"The magnification mode may not be set to use mipmaps");

		Image image = filepath;
		auto& metadata = image.GetMetadata();

		m_Spec.Width          = metadata.Width;
		m_Spec.Height         = metadata.Height;
		m_Spec.DataFormat     = metadata.DataFormat;
		m_Spec.DataFormat     = metadata.DataFormat;
		m_Spec.RenderModes    = renderModes;

		QK_CORE_ASSERT(m_Spec.Width <= Renderer::GetCapabilities().TextureCapabilities.MaxPixelSize
			&& m_Spec.Height <= Renderer::GetCapabilities().TextureCapabilities.MaxPixelSize, "Texture dimensions too large");

		m_DataFormat = DataFormatToOpenGLStorageFormat(m_Spec.DataFormat);
		m_InternalFormat = DataFormatToOpenGLInternalFormat(m_Spec.DataFormat);
		QK_CORE_ASSERT(m_InternalFormat & m_DataFormat, "Image format not supported");

		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);
		glPixelStorei(GL_UNPACK_ALIGNMENT, IsPixel4BytesAligned(m_Spec.DataFormat) ? 4 : 1);
		glTexImage2D(GL_TEXTURE_2D, m_Spec.Levels,
			m_InternalFormat, m_Spec.Width, m_Spec.Height, 0, m_DataFormat,
			DataFormatToOpenGLDataType(m_Spec.DataFormat), *image);

		GLenum tilingMode = TilingModeToOpenGL(m_Spec.RenderModes.TilingMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, FilteringModeToOpenGL(m_Spec.RenderModes.MinFilteringMode));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, FilteringModeToOpenGL(m_Spec.RenderModes.MagFilteringMode));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, tilingMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tilingMode);

		if (IsformatUsingMips(m_Spec.RenderModes.MinFilteringMode))
		{
			glGenerateMipmap(GL_TEXTURE_2D);
		}

		QK_DEBUG_CALL(glBindTexture(GL_TEXTURE_2D, 0));
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		QK_PROFILE_FUNCTION();

		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLTexture2D::Attach(uint32_t textureSlot) const
	{
		GLenum target = m_Spec.Samples > 1 ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
		glActiveTexture(GL_TEXTURE0 + textureSlot);
		glBindTexture(target, m_RendererID);
	}

	void OpenGLTexture2D::Detach() const
	{
		GLenum target = m_Spec.Samples > 1 ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
		glBindTexture(target, 0);
	}

	void OpenGLTexture2D::SetData(const void* data, size_t size)
	{
		QK_PROFILE_FUNCTION();

		size_t pSize = GetPixelFormatSize(m_Spec.DataFormat);
		QK_CORE_ASSERT(size == m_Spec.Width * m_Spec.Height * pSize, "Data must be entire texture");

		GLenum target = m_Spec.Samples > 1 ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
		glBindTexture(target, m_RendererID);
		glPixelStorei(GL_UNPACK_ALIGNMENT, IsPixel4BytesAligned(m_Spec.DataFormat) ? 4 : 1);
		glTexSubImage2D(target, 0, 0, 0, m_Spec.Width, m_Spec.Height, m_DataFormat,
			DataFormatToOpenGLDataType(m_Spec.DataFormat), data);

		QK_DEBUG_CALL(glBindTexture(target, 0));
	}

	void OpenGLTexture2D::GenerateMipmaps()
	{
		QK_PROFILE_FUNCTION();

		QK_CORE_ASSERT(IsformatUsingMips(m_Spec.RenderModes.MinFilteringMode),
			"Invalid texture specification for mipmaps");

		GLenum target = m_Spec.Samples > 1 ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
		glBindTexture(target, m_RendererID);
		glGenerateMipmap(target);

		QK_DEBUG_CALL(glBindTexture(target, 0));
	}

	bool OpenGLTexture2D::operator==(const Texture& other) const
	{
		if (auto o = dynamic_cast<decltype(this)>(&other))
			return m_RendererID == o->m_RendererID;

		return false;
	}
}
