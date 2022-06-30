#include "qkpch.h"
#include "OpenGLTexture.h"
#include "OpenGLTextureFormats.h"

#include "Quark/Renderer/Image.h"
#include "Quark/Renderer/GraphicsAPI.h"

#include <glad/glad.h>

namespace Quark {

	OpenGLTexture2D::OpenGLTexture2D(const Texture2DSpecification& spec)
		: m_Spec(spec)
	{
		QK_PROFILE_FUNCTION();

		QK_CORE_ASSERT(m_Spec.Width <= GraphicsAPI::Instance->GetHardwareConstraints().TextureConstraints.MaxPixelSize
			&& m_Spec.Height <= GraphicsAPI::Instance->GetHardwareConstraints().TextureConstraints.MaxPixelSize, "Texture dimensions too large");

		m_InternalFormat = GetTextureInternalFormat(m_Spec.InternalFormat);
		m_DataFormat = GetTextureDataFormat(m_Spec.DataFormat);
		QK_CORE_ASSERT(m_InternalFormat & m_DataFormat, "Image format not supported");

		glGenTextures(1, &m_RendererID);

		bool multisampled = m_Spec.Samples > 1;
		if (multisampled)
		{
			glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_RendererID);
			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_Spec.Samples, m_InternalFormat, m_Spec.Width, m_Spec.Height, GL_FALSE);
		}
		else
		{
			glBindTexture(GL_TEXTURE_2D, m_RendererID);
			glTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, m_Spec.Width, m_Spec.Height, 0, m_DataFormat,
				GetDataTypeBasedOnInternalFormat(m_Spec.InternalFormat), nullptr);

			GLenum tilingMode = GetTextureTilingMode(m_Spec.RenderModes.TilingMode);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GetTextureFilteringMode(m_Spec.RenderModes.MinFilteringMode));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GetTextureFilteringMode(m_Spec.RenderModes.MagFilteringMode));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, tilingMode);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tilingMode);
		}
	}

	OpenGLTexture2D::OpenGLTexture2D(std::string_view filepath, const TextureFormatDescriptor& descriptor)
	{
		QK_PROFILE_FUNCTION();

		Image image = filepath;
		auto& metadata = image.GetMetadata();

		m_Spec.Width          = metadata.Width;
		m_Spec.Height         = metadata.Height;
		m_Spec.DataFormat     = metadata.DataFormat;
		m_Spec.InternalFormat = metadata.InternalFormat;
		m_Spec.RenderModes    = descriptor.RenderModes;

		QK_CORE_ASSERT(m_Spec.Width <= GraphicsAPI::Instance->GetHardwareConstraints().TextureConstraints.MaxPixelSize
			&& m_Spec.Height <= GraphicsAPI::Instance->GetHardwareConstraints().TextureConstraints.MaxPixelSize, "Texture dimensions too large");

		m_DataFormat = GetTextureDataFormat(m_Spec.DataFormat);
		m_InternalFormat = GetTextureInternalFormat(m_Spec.InternalFormat);
		QK_CORE_ASSERT(m_InternalFormat & m_DataFormat, "Image format not supported");

		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);
		glTexImage2D(GL_TEXTURE_2D, 0,
			m_InternalFormat, m_Spec.Width, m_Spec.Height, 0, m_DataFormat,
			GetDataTypeBasedOnInternalFormat(m_Spec.InternalFormat), *image);

		GLenum tilingMode = GetTextureTilingMode(m_Spec.RenderModes.TilingMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GetTextureFilteringMode(m_Spec.RenderModes.MinFilteringMode));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GetTextureFilteringMode(m_Spec.RenderModes.MagFilteringMode));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, tilingMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tilingMode);

		if (IsformatUsingMips(m_Spec.RenderModes.MinFilteringMode) || IsformatUsingMips(m_Spec.RenderModes.MagFilteringMode))
		{
			glGenerateMipmap(GL_TEXTURE_2D);
		}
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

		size_t pSize = GetPixelFormatSize(m_Spec.InternalFormat);
		QK_CORE_ASSERT(size == m_Spec.Width * m_Spec.Height * pSize, "Data must be entire texture");

		GLenum target = m_Spec.Samples > 1 ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
		glBindTexture(target, m_RendererID);
		glTexSubImage2D(target, 0, 0, 0, m_Spec.Width, m_Spec.Height, m_DataFormat,
			GetDataTypeBasedOnInternalFormat(m_Spec.InternalFormat), data);
	}

	void OpenGLTexture2D::GenerateMipmaps()
	{
		QK_PROFILE_FUNCTION();

		QK_CORE_ASSERT(IsformatUsingMips(m_Spec.RenderModes.MinFilteringMode) || IsformatUsingMips(m_Spec.RenderModes.MagFilteringMode),
			"Invalid texture specification for mipmaps");

		glGenerateMipmap(GL_TEXTURE_2D);
	}
}
