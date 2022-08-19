#include "qkpch.h"
#include "OpenGLCubemap.h"
#include "OpenGLFormats.h"

#include <glad/glad.h>

namespace Quark {

	OpenGLCubemap::OpenGLCubemap(const CubemapSpecification& spec)
		: Cubemap(spec)
	{
		QK_PROFILE_FUNCTION();

		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);
		glPixelStorei(GL_UNPACK_ALIGNMENT, IsPixel4BytesAligned(m_Spec.DataFormat) ? 4 : 1);

		for (uint8_t i = 0; i < 6; i++)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
				DataFormatToOpenGLInternalFormat(m_Spec.DataFormat), m_Spec.Width, m_Spec.Height, 0,
				DataFormatToOpenGLStorageFormat(m_Spec.DataFormat),
				DataFormatToOpenGLDataType(m_Spec.DataFormat), nullptr);
		}

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		QK_DEBUG_CALL(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));
	}

	OpenGLCubemap::~OpenGLCubemap()
	{
		QK_PROFILE_FUNCTION();

		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLCubemap::Attach(uint32_t textureSlot) const
	{
		glActiveTexture(GL_TEXTURE0 + textureSlot);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);
	}

	void OpenGLCubemap::Detach() const
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}

	void OpenGLCubemap::SetData(uint32_t index, const void* data, size_t size)
	{
		QK_PROFILE_FUNCTION();

		size_t pSize = GetPixelFormatSize(m_Spec.DataFormat);
		QK_CORE_ASSERT(size == m_Spec.Width * m_Spec.Height * pSize, "Data must be entire texture");
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);
		glPixelStorei(GL_UNPACK_ALIGNMENT, IsPixel4BytesAligned(m_Spec.DataFormat) ? 4 : 1);
		glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + index, 0, 0, 0, m_Spec.Width, m_Spec.Height,
			DataFormatToOpenGLStorageFormat(m_Spec.DataFormat),
			DataFormatToOpenGLDataType(m_Spec.DataFormat), data);
	}
}
