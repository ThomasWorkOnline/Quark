#include "OpenGLTexture.h"
#include "OpenGLTextureFormats.h"

#include "../../Quark/Renderer/Image.h"

#include <glad/glad.h>

namespace Quark {

	static bool IsPowerOfTwo(uint32_t x)
	{
		return (x & (x - 1)) == 0;
	}

	OpenGLTexture2D::OpenGLTexture2D(const TextureSpecification& spec)
		: m_Spec(spec)
	{
		m_InternalFormat = GetTextureInternalFormat(m_Spec.DataFormat);
		m_DataFormat = GetTextureDataFormat(m_Spec.DataFormat);

		glGenTextures(1, &m_RendererID);
		glActiveTexture(GL_TEXTURE0);

		bool multisampled = m_Spec.Samples > 1;
		if (multisampled)
		{
			glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_RendererID);
			glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_Spec.Samples, m_InternalFormat, m_Spec.Width, m_Spec.Height, GL_FALSE);

			GLenum tilingMode = GetTextureTilingMode(m_Spec.RenderModes.TilingMode);
			glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GetTextureFilteringMode(m_Spec.RenderModes.MinFilteringMode));
			glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GetTextureFilteringMode(m_Spec.RenderModes.MagFilteringMode));
			glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_S, tilingMode);
			glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_T, tilingMode);
		}
		else
		{
			glBindTexture(GL_TEXTURE_2D, m_RendererID);

			// Immutable texture format, contents can change but not specification
			glTexStorage2D(GL_TEXTURE_2D, 1, m_InternalFormat, m_Spec.Width, m_Spec.Height);

			GLenum tilingMode = GetTextureTilingMode(m_Spec.RenderModes.TilingMode);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GetTextureFilteringMode(m_Spec.RenderModes.MinFilteringMode));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GetTextureFilteringMode(m_Spec.RenderModes.MagFilteringMode));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, tilingMode);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tilingMode);
		}
	}

	OpenGLTexture2D::OpenGLTexture2D(const std::string& filepath, const TextureRenderModes& modes)
	{
		Image image(filepath, true);
		const ImageProperties& prop = image.GetProperties();

		m_Spec.Width = prop.Width;
		m_Spec.Height = prop.Height;
		m_Spec.RenderModes = modes;

		GLenum internalFormat = 0, dataFormat = 0;
		if (prop.Channels == 4)
		{
			internalFormat = GL_SRGB_ALPHA;
			dataFormat = GL_RGBA;
		}
		else if (prop.Channels == 3)
		{
			internalFormat = GL_SRGB;
			dataFormat = GL_RGB;
		}

		m_InternalFormat = internalFormat;
		m_DataFormat = dataFormat;
		QK_ASSERT(internalFormat & dataFormat, "Image format not supported");

		glGenTextures(1, &m_RendererID);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);

		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_Spec.Width, m_Spec.Height, 0, dataFormat, GL_UNSIGNED_BYTE, image.Data());

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

	void OpenGLTexture2D::SetData(void* data, uint32_t size)
	{
		bool alpha = IsTextureAlphaFormat(m_Spec.DataFormat);
		uint32_t bpp = alpha ? 4 : 3;
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
