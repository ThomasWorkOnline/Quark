#include "qkpch.h"
#include "OpenGLTextureArray.h"
#include "OpenGLFormats.h"

#include <glad/glad.h>

namespace Quark {

	OpenGLTexture2DArray::OpenGLTexture2DArray(const Texture2DArraySpecification& spec) : Texture2DArray(spec)
	{
		QK_PROFILE_FUNCTION();

		QK_CORE_ASSERT(!IsformatUsingMips(spec.RenderModes.MagFilteringMode),
			"The magnification mode may not be set to use mipmaps");

		m_InternalFormat = DataFormatToOpenGLInternalFormat(m_Spec.DataFormat);
		m_DataFormat = DataFormatToOpenGLStorageFormat(m_Spec.DataFormat);

		glGenTextures(1, &m_RendererID);

		bool multisampled = m_Spec.Samples > 1;
		if (multisampled)
		{
			QK_CORE_ASSERT(m_Spec.Levels == 0, "Mipmap level must be set to 0 when multisampling is enabled");

			glBindTexture(GL_TEXTURE_2D_MULTISAMPLE_ARRAY, m_RendererID);
			glPixelStorei(GL_UNPACK_ALIGNMENT, IsPixel4BytesAligned(m_Spec.DataFormat) ? 4 : 1);
			glTexImage3DMultisample(GL_TEXTURE_2D_MULTISAMPLE_ARRAY, m_Spec.Samples, m_InternalFormat, m_Spec.Width, m_Spec.Height, m_Spec.Layers, GL_FALSE);

			QK_DEBUG_CALL(glBindTexture(GL_TEXTURE_2D_MULTISAMPLE_ARRAY, 0));
		}
		else
		{
			QK_CORE_ASSERT(IsformatUsingMips(m_Spec.RenderModes.MinFilteringMode) == m_Spec.Levels > 0,
				"Mipmap level must match the texture minification mode");

			glBindTexture(GL_TEXTURE_2D_ARRAY, m_RendererID);
			glPixelStorei(GL_UNPACK_ALIGNMENT, IsPixel4BytesAligned(m_Spec.DataFormat) ? 4 : 1);
			glTexImage3D(GL_TEXTURE_2D_ARRAY, m_Spec.Levels, m_InternalFormat, m_Spec.Width, m_Spec.Height, m_Spec.Layers, 0,
				m_DataFormat, DataFormatToOpenGLDataType(m_Spec.DataFormat), nullptr);

			GLenum tilingMode = TilingModeToOpenGL(m_Spec.RenderModes.TilingMode);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, FilteringModeToOpenGL(m_Spec.RenderModes.MinFilteringMode));
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, FilteringModeToOpenGL(m_Spec.RenderModes.MagFilteringMode));
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, tilingMode);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, tilingMode);

			QK_DEBUG_CALL(glBindTexture(GL_TEXTURE_2D_ARRAY, 0));
		}
	}

	OpenGLTexture2DArray::~OpenGLTexture2DArray()
	{
		QK_PROFILE_FUNCTION();

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
		QK_PROFILE_FUNCTION();

		size_t pSize = GetPixelFormatSize(m_Spec.DataFormat);
		QK_CORE_ASSERT(size == m_Spec.Width * m_Spec.Height * pSize, "Data must be entire texture");

		GLenum target = m_Spec.Samples > 1 ? GL_TEXTURE_2D_MULTISAMPLE_ARRAY : GL_TEXTURE_2D_ARRAY;
		glBindTexture(target, m_RendererID);
		glPixelStorei(GL_UNPACK_ALIGNMENT, IsPixel4BytesAligned(m_Spec.DataFormat) ? 4 : 1);
		glTexSubImage3D(target, m_Spec.Levels, 0, 0, layer, m_Spec.Width, m_Spec.Height, 1,
			m_DataFormat, DataFormatToOpenGLDataType(m_Spec.DataFormat), data);

		QK_DEBUG_CALL(glBindTexture(target, 0));
	}

	void OpenGLTexture2DArray::GenerateMipmaps()
	{
		QK_PROFILE_FUNCTION();

		QK_CORE_ASSERT(IsformatUsingMips(m_Spec.RenderModes.MinFilteringMode),
			"Invalid texture specification for mipmaps");

		GLenum target = m_Spec.Samples > 1 ? GL_TEXTURE_2D_MULTISAMPLE_ARRAY : GL_TEXTURE_2D_ARRAY;
		glBindTexture(target, m_RendererID);
		glGenerateMipmap(target);

		QK_DEBUG_CALL(glBindTexture(target, 0));
	}
}
