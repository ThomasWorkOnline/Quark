#include "qkpch.h"
#include "OpenGLTexture.h"
#include "OpenGLTextureFormats.h"

#include "Quark/Renderer/Image.h"

#include <glad/glad.h>

namespace Quark {

	static constexpr bool IsPowerOfTwo(uint32_t x)
	{
		return (x & (x - 1)) == 0;
	}

	OpenGLTexture2D::OpenGLTexture2D(const Texture2DSpecification& spec)
		: m_Spec(spec)
	{
		QK_PROFILE_FUNCTION();

		m_InternalFormat = GetTextureInternalFormat(m_Spec.InternalFormat);
		m_DataFormat = GetTextureDataFormat(m_Spec.DataFormat);

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

		Ref<Image> image = Image::Create(filepath);

		m_Spec.Width       = image->Width();
		m_Spec.Height      = image->Height();
		m_Spec.RenderModes = descriptor.RenderModes;

		if (image->IsHDR())
		{
			m_DataType = GL_FLOAT;
			switch (image->Channels())
			{
				case 3:
					m_InternalFormat = GL_RGB16F;
					m_DataFormat = GL_RGB;
					break;
				case 4:
					m_InternalFormat = GL_RGBA16F;
					m_DataFormat = GL_RGBA;
					break;
				default:
					QK_CORE_ASSERT(false, "Image format not supported");
					break;
			}
		}
		else
		{
			m_DataType = GL_UNSIGNED_BYTE;
			switch (image->Channels())
			{
				case 1:
					m_InternalFormat = GL_R8;
					m_DataFormat = GL_RED;
					break;
				case 3:
					m_InternalFormat = descriptor.SRGB ? GL_SRGB8 : GL_RGB8;
					m_DataFormat = GL_RGB;
					break;
				case 4:
					m_InternalFormat = descriptor.SRGB ? GL_SRGB8_ALPHA8 : GL_RGBA8;
					m_DataFormat = GL_RGBA;
					break;
				default:
					QK_CORE_ASSERT(false, "Image format not supported");
					break;
			}
		}

		QK_CORE_ASSERT(m_InternalFormat & m_DataFormat, "Image format not supported");

		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);
		glTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, m_Spec.Width, m_Spec.Height, 0, m_DataFormat, m_DataType, image->Data());

		GLenum tilingMode = GetTextureTilingMode(m_Spec.RenderModes.TilingMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GetTextureFilteringMode(m_Spec.RenderModes.MinFilteringMode));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GetTextureFilteringMode(m_Spec.RenderModes.MagFilteringMode));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, tilingMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tilingMode);

		if (TextureHasMips(m_Spec.RenderModes.MinFilteringMode) || TextureHasMips(m_Spec.RenderModes.MagFilteringMode))
		{
			glGenerateMipmap(GL_TEXTURE_2D);
		}
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
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
		QK_CORE_ASSERT(TextureHasMips(m_Spec.RenderModes.MinFilteringMode) || TextureHasMips(m_Spec.RenderModes.MagFilteringMode),
			"Invalid texture specification for mipmaps");

		glGenerateMipmap(GL_TEXTURE_2D);
	}
}
