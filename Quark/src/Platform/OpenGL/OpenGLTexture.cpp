#include "OpenGLTexture.h"
#include "OpenGLTextureFormats.h"

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include "../../../vendor/stb_image/stb_image.h"

#include <GL/glew.h>

namespace Quark {

	OpenGLTexture2D::OpenGLTexture2D(const TextureSpecification& spec)
		: m_Spec(spec)
	{
		glGenTextures(1, &m_RendererID);

		m_InternalFormat = GetTextureInternalFormat(m_Spec.TextureFormat);
		m_DataFormat = GetTextureDataFormat(m_Spec.TextureFormat);

		bool multisampled = m_Spec.Samples > 1;
		if (multisampled)
		{
			glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_RendererID);
			glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_Spec.Samples, m_InternalFormat, m_Spec.Width, m_Spec.Height, GL_FALSE);

			glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GetTextureFilteringFormat(m_Spec.FilteringFormat));
			glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GetTextureFilteringFormat(m_Spec.FilteringFormat));
			glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_S, GetTextureTilingFormat(m_Spec.TilingFormat));
			glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_T, GetTextureTilingFormat(m_Spec.TilingFormat));
		}
		else
		{
			glBindTexture(GL_TEXTURE_2D, m_RendererID);

			// Immutable texture format, contents can change but not specification
			glTexStorage2D(GL_TEXTURE_2D, 1, m_InternalFormat, m_Spec.Width, m_Spec.Height);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GetTextureFilteringFormat(m_Spec.FilteringFormat));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GetTextureFilteringFormat(m_Spec.FilteringFormat));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GetTextureTilingFormat(m_Spec.TilingFormat));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GetTextureTilingFormat(m_Spec.TilingFormat));
		}
	}

	OpenGLTexture2D::OpenGLTexture2D(const std::string& filepath)
		: m_Filepath(filepath)
	{
		int32_t width, height, channels;
		stbi_set_flip_vertically_on_load(true);
		stbi_uc* data = stbi_load(filepath.c_str(), &width, &height, &channels, 0);
		QK_ASSERT(data, "Failed to load image at path: " << filepath);

		m_Spec.Width = width;
		m_Spec.Height = height;

		GLenum internalFormat = 0, dataFormat = 0;
		if (channels == 4)
		{
			internalFormat = GL_SRGB_ALPHA;
			dataFormat = GL_RGBA;
		}
		else if (channels == 3)
		{
			internalFormat = GL_SRGB;
			dataFormat = GL_RGB;
		}

		m_InternalFormat = internalFormat;
		m_DataFormat = dataFormat;
		QK_ASSERT(internalFormat & dataFormat, "Image format not supported");

		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);

		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glGenerateMipmap(GL_TEXTURE_2D);

		stbi_image_free(data);
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLTexture2D::SetData(void* data, uint32_t size)
	{
		uint32_t bpp = m_DataFormat == GL_RGBA ? 4 : 3;
		QK_ASSERT(size == m_Spec.Width * m_Spec.Height * bpp, "Data must be entire texture");
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_Spec.Width, m_Spec.Height, m_DataFormat, GL_UNSIGNED_BYTE, data);
	}

	void OpenGLTexture2D::Attach(uint32_t textureSlot) const
	{
		glActiveTexture(GL_TEXTURE0 + textureSlot);
		glBindTexture(GetTarget(), m_RendererID);
	}

	void OpenGLTexture2D::Detach() const
	{
		glBindTexture(GetTarget(), 0);
	}

	uint32_t OpenGLTexture2D::GetTarget() const
	{
		if (m_Spec.Samples == 1)
			return GL_TEXTURE_2D;
		else
			return GL_TEXTURE_2D_MULTISAMPLE;
	}
}
