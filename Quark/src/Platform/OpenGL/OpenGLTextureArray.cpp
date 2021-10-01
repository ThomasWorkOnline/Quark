#include "OpenGLTextureArray.h"
#include "OpenGLTextureFormats.h"

#include <GL/glew.h>

namespace Quark {

	OpenGLTexture2DArray::OpenGLTexture2DArray(const TextureArraySpecification& spec)
		: m_Spec(spec)
	{
		// TODO: implement multisampling
		QK_ASSERT(spec.Samples == 1, "OpenGLTexture2DArray does not support multisampling yet");

		m_InternalFormat = GetTextureInternalFormat(m_Spec.DataFormat);
		m_DataFormat = GetTextureDataFormat(m_Spec.DataFormat);

		glGenTextures(1, &m_RendererID);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D_ARRAY, m_RendererID);

		glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, m_InternalFormat, m_Spec.Width, m_Spec.Height, m_Spec.Layers);

		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GetTextureFilteringMode(m_Spec.RenderModes.MinFilteringMode));
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GetTextureFilteringMode(m_Spec.RenderModes.MagFilteringMode));
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GetTextureTilingMode(m_Spec.RenderModes.TilingMode));
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GetTextureTilingMode(m_Spec.RenderModes.TilingMode));
	}

	OpenGLTexture2DArray::~OpenGLTexture2DArray()
	{
		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLTexture2DArray::SetData(void* data, uint32_t size, uint32_t layer)
	{
		bool alpha = IsTextureAlphaFormat(m_Spec.DataFormat);
		uint32_t bpp = alpha ? 4 : 3;
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
