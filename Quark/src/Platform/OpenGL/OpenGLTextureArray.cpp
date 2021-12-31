#include "OpenGLTextureArray.h"
#include "OpenGLTextureFormats.h"

#include <glad/glad.h>

namespace Quark {

	OpenGLTexture2DArray::OpenGLTexture2DArray(const TextureArraySpecification& spec)
		: m_Spec(spec)
	{
		// TODO: implement multisampling
		QK_ASSERT(spec.Samples == 1, "OpenGLTexture2DArray does not yet support multisampling");

		m_InternalFormat = GetTextureInternalFormat(m_Spec.DataFormat);
		m_DataFormat = GetTextureDataFormat(m_Spec.DataFormat);

		glGenTextures(1, &m_RendererID);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D_ARRAY, m_RendererID);

		glTexStorage3D(GL_TEXTURE_2D_ARRAY, spec.Levels, m_InternalFormat, m_Spec.Width, m_Spec.Height, m_Spec.Layers);

		GLenum tilingMode = GetTextureTilingMode(m_Spec.RenderModes.TilingMode);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GetTextureFilteringMode(m_Spec.RenderModes.MinFilteringMode));
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GetTextureFilteringMode(m_Spec.RenderModes.MagFilteringMode));
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, tilingMode);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, tilingMode);
	}

	OpenGLTexture2DArray::~OpenGLTexture2DArray()
	{
		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLTexture2DArray::SetData(void* data, size_t size, uint32_t layer)
	{
		bool alpha = IsTextureAlphaFormat(m_Spec.DataFormat);
		uint32_t bpp = alpha ? 4 : 3;
		QK_ASSERT(size == m_Spec.Width * m_Spec.Height * bpp, "Data must be entire texture");
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, layer, m_Spec.Width, m_Spec.Height, 1, m_DataFormat, GL_UNSIGNED_BYTE, data);
	}

	void OpenGLTexture2DArray::GenerateMipmaps()
	{
		glBindTexture(GL_TEXTURE_2D_ARRAY, m_RendererID);
		glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
	}

	void OpenGLTexture2DArray::Attach(uint32_t textureSlot) const
	{
		glActiveTexture(GL_TEXTURE0 + textureSlot);
		glBindTexture(GL_TEXTURE_2D_ARRAY, m_RendererID);
	}

	void OpenGLTexture2DArray::Detach() const
	{
		glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
	}
}
