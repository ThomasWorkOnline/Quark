#include "OpenGLTextureArray.h"
#include "OpenGLTextureFormats.h"

#include <GL/glew.h>

namespace Quark {

	OpenGLTexture2DArray::OpenGLTexture2DArray(const TextureArraySpecification& spec)
		: m_Spec(spec)
	{
		QK_ASSERT(spec.Samples == 1, "OpenGLTexture2DArray does not support multisampling yet");

		m_InternalFormat = GetTextureInternalFormat(m_Spec.TextureFormat);
		m_DataFormat = GetTextureDataFormat(m_Spec.TextureFormat);

		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_2D_ARRAY, m_RendererID);

		glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, m_InternalFormat, m_Spec.Width, m_Spec.Height, m_Spec.Layers);

		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GetTextureFilteringFormat(m_Spec.MinFilteringFormat));
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GetTextureFilteringFormat(m_Spec.MagFilteringFormat));
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GetTextureTilingFormat(m_Spec.TilingFormat));
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GetTextureTilingFormat(m_Spec.TilingFormat));
	}

	OpenGLTexture2DArray::~OpenGLTexture2DArray()
	{
		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLTexture2DArray::SetData(void* data, uint32_t size, uint32_t layer)
	{
		uint32_t bpp = m_DataFormat == GL_RGBA ? 4 : 3;
		QK_ASSERT(size == m_Spec.Width * m_Spec.Height * bpp, "Data must be entire texture");
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, layer, m_Spec.Width, m_Spec.Height, m_Spec.Layers, m_DataFormat, GL_UNSIGNED_BYTE, data);
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
