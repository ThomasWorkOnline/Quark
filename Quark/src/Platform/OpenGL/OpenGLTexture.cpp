#include "OpenGLTexture.h"
#include "OpenGLTextureFormats.h"

#include "Quark/Renderer/Image.h"

#include <glad/glad.h>

#define STB_IMAGE_WRITE_STATIC
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

namespace Quark {

	static constexpr bool IsPowerOfTwo(uint32_t x)
	{
		return (x & (x - 1)) == 0;
	}

	OpenGLTexture2D::OpenGLTexture2D(const TextureSpecification& spec)
		: m_Spec(spec)
	{
		m_InternalFormat = GetTextureInternalFormat(m_Spec.InternalFormat);
		m_DataFormat = GetTextureFormat(m_Spec.DataFormat);

		glGenTextures(1, &m_RendererID);
		glActiveTexture(GL_TEXTURE0);

		bool multisampled = m_Spec.Samples > 1;
		if (multisampled)
		{
			glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_RendererID);
			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_Spec.Samples, m_InternalFormat, m_Spec.Width, m_Spec.Height, GL_FALSE);
		}
		else
		{
			glBindTexture(GL_TEXTURE_2D, m_RendererID);
			glTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, m_Spec.Width, m_Spec.Height, 0, m_DataFormat, GL_UNSIGNED_BYTE, nullptr);
		}

		GLenum target     = GetTextureSampleTarget(m_Spec.Samples);
		GLenum tilingMode = GetTextureTilingMode(m_Spec.RenderModes.TilingMode);
		glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GetTextureFilteringMode(m_Spec.RenderModes.MinFilteringMode));
		glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GetTextureFilteringMode(m_Spec.RenderModes.MagFilteringMode));
		glTexParameteri(target, GL_TEXTURE_WRAP_S, tilingMode);
		glTexParameteri(target, GL_TEXTURE_WRAP_T, tilingMode);
	}

	OpenGLTexture2D::OpenGLTexture2D(std::string_view filepath, const TextureRenderModes& modes)
	{
		Image image(filepath);
		const ImageProperties& props = image.GetProperties();

		m_Spec.Width = props.Width;
		m_Spec.Height = props.Height;
		m_Spec.RenderModes = modes;

		GLenum internalFormat = 0, dataFormat = 0;
		switch (props.Channels)
		{
		case 1:
			internalFormat = GL_R8;
			dataFormat = GL_RED;
			break;
		case 3:
			internalFormat = props.SRGB ? GL_SRGB8 : GL_RGB8;
			dataFormat = GL_RGB;
			break;
		case 4:
			internalFormat = props.SRGB ? GL_SRGB8_ALPHA8 : GL_RGBA8;
			dataFormat = GL_RGBA;
			break;
		}

		m_InternalFormat = internalFormat;
		m_DataFormat = dataFormat;
		QK_CORE_ASSERT(internalFormat & dataFormat, "Image format not supported");

		glGenTextures(1, &m_RendererID);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_Spec.Width, m_Spec.Height, 0, dataFormat, GL_UNSIGNED_BYTE, *image);

		GLenum tilingMode = GetTextureTilingMode(m_Spec.RenderModes.TilingMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GetTextureFilteringMode(m_Spec.RenderModes.MinFilteringMode));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GetTextureFilteringMode(m_Spec.RenderModes.MagFilteringMode));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, tilingMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tilingMode);

		glGenerateMipmap(GL_TEXTURE_2D);
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLTexture2D::SetData(const void* data, size_t size)
	{
		bool alpha = IsTextureAlphaFormat(m_Spec.DataFormat);
		uint32_t bpp = alpha ? 4 : 3;
		QK_CORE_ASSERT(size == m_Spec.Width * m_Spec.Height * bpp, "Data must be entire texture");

		glBindTexture(GetTextureSampleTarget(m_Spec.Samples), m_RendererID);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_Spec.Width, m_Spec.Height, m_DataFormat, GL_UNSIGNED_BYTE, data);
	}

	void OpenGLTexture2D::Attach(uint32_t textureSlot) const
	{
		glActiveTexture(GL_TEXTURE0 + textureSlot);
		glBindTexture(GetTextureSampleTarget(m_Spec.Samples), m_RendererID);
	}

	void OpenGLTexture2D::Detach() const
	{
		glBindTexture(GetTextureSampleTarget(m_Spec.Samples), 0);
	}
}
