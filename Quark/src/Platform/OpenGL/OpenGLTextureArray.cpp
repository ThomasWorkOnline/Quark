#include "qkpch.h"
#include "OpenGLTextureArray.h"
#include "OpenGLTextureFormats.h"

#include <glad/glad.h>

namespace Quark {

	OpenGLTexture2DArray::OpenGLTexture2DArray(const TextureArraySpecification& spec)
		: m_Spec(spec)
	{
		QK_PROFILE_FUNCTION();

		m_InternalFormat = GetTextureInternalFormat(m_Spec.InternalFormat);
		m_DataFormat = GetTextureDataFormat(m_Spec.DataFormat);

		glGenTextures(1, &m_RendererID);

		bool multisampled = m_Spec.Samples > 1;
		if (multisampled)
		{
			glBindTexture(GL_TEXTURE_2D_MULTISAMPLE_ARRAY, m_RendererID);
			glTexImage3DMultisample(GL_TEXTURE_2D_MULTISAMPLE_ARRAY, m_Spec.Samples, m_InternalFormat, m_Spec.Width, m_Spec.Height, m_Spec.Layers, GL_FALSE);
		}
		else
		{
			glBindTexture(GL_TEXTURE_2D_ARRAY, m_RendererID);
			glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, m_InternalFormat, m_Spec.Width, m_Spec.Height, m_Spec.Layers, 0, m_DataFormat, GL_UNSIGNED_BYTE, nullptr);

			GLenum tilingMode = GetTextureTilingMode(m_Spec.RenderModes.TilingMode);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GetTextureFilteringMode(m_Spec.RenderModes.MinFilteringMode));
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GetTextureFilteringMode(m_Spec.RenderModes.MagFilteringMode));
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, tilingMode);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, tilingMode);
		}
	}

	OpenGLTexture2DArray::~OpenGLTexture2DArray()
	{
		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLTexture2DArray::Attach(uint32_t textureSlot) const
	{
		GLenum target = m_Spec.Samples > 1 ? GL_TEXTURE_2D_MULTISAMPLE_ARRAY : GL_TEXTURE_2D_ARRAY;
		glActiveTexture(GL_TEXTURE0 + textureSlot);
		glBindTexture(target, m_RendererID);
	}

	void OpenGLTexture2DArray::Detach() const
	{
		GLenum target = m_Spec.Samples > 1 ? GL_TEXTURE_2D_MULTISAMPLE_ARRAY : GL_TEXTURE_2D_ARRAY;
		glBindTexture(target, 0);
	}

	void OpenGLTexture2DArray::SetData(const void* data, size_t size, uint32_t layer)
	{
		size_t pSize = GetPixelFormatSize(m_Spec.InternalFormat);
		QK_CORE_ASSERT(size == m_Spec.Width * m_Spec.Height * pSize, "Data must be entire texture");

		GLenum target = m_Spec.Samples > 1 ? GL_TEXTURE_2D_MULTISAMPLE_ARRAY : GL_TEXTURE_2D_ARRAY;
		glBindTexture(target, m_RendererID);
		glTexSubImage3D(target, 0, 0, 0, layer, m_Spec.Width, m_Spec.Height, 1, m_DataFormat, GL_UNSIGNED_BYTE, data);
	}

	void OpenGLTexture2DArray::GenerateMipmaps()
	{
		QK_PROFILE_FUNCTION();

		QK_ASSERT(m_Spec.Samples == 1, "Texture is multisampled. Mipmaps are not intended");

		glBindTexture(GL_TEXTURE_2D_ARRAY, m_RendererID);
		glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
	}
}
