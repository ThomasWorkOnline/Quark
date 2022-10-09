#include "qkpch.h"
#include "OpenGLTexture.h"
#include "OpenGLEnums.h"

#include "Quark/Renderer/Image.h"
#include "Quark/Renderer/Renderer.h"

#include <glad/glad.h>

namespace Quark {

	OpenGLTexture2D::OpenGLTexture2D(const Texture2DSpecification& spec)
		: Texture2D(spec)
	{
		QK_PROFILE_FUNCTION();

		QK_CORE_ASSERT(m_Spec.Width <= Renderer::GetCapabilities().Texture.Max2DSize
			&& m_Spec.Height <= Renderer::GetCapabilities().Texture.Max2DSize,
			"Texture dimensions too large: see Renderer::GetCapabilities() for more info");

		m_Spec.Levels = m_Spec.Levels == 0 ? GetMipLevelsForResolution(m_Spec.Width, m_Spec.Height) : m_Spec.Levels;

		m_InternalFormat = DataFormatToOpenGLInternalFormat(m_Spec.DataFormat);
		m_DataFormat = DataFormatToOpenGLStorageFormat(m_Spec.DataFormat);
		QK_CORE_ASSERT(m_InternalFormat & m_DataFormat, "Invalid internal and data format combinaison");

		bool multisampled = m_Spec.Samples > 1;
		m_Target = multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;

		glGenTextures(1, &m_RendererID);
		glBindTexture(m_Target, m_RendererID);
		glPixelStorei(GL_UNPACK_ALIGNMENT, IsPixel4BytesAligned(m_Spec.DataFormat) ? 4 : 1);

		if (multisampled)
		{
			QK_CORE_ASSERT(m_Spec.Levels == 1, "Mipmaps are not compatible when multisampling is enabled");
			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_Spec.Samples, m_InternalFormat, m_Spec.Width, m_Spec.Height, GL_FALSE);
		}
		else
		{
			glTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, m_Spec.Width, m_Spec.Height, 0, m_DataFormat,
				DataFormatToOpenGLDataType(m_Spec.DataFormat), NULL);
		}

		QK_CORE_ASSERT(glIsTexture(m_RendererID), "Texture is incomplete!");
		QK_DEBUG_CALL(glBindTexture(m_Target, 0));
	}

	OpenGLTexture2D::OpenGLTexture2D(std::string_view filepath)
	{
		QK_PROFILE_FUNCTION();

		Image image = filepath;
		auto& metadata = image.GetMetadata();

		QK_CORE_ASSERT(metadata.Width <= Renderer::GetCapabilities().Texture.Max2DSize
			&& metadata.Height <= Renderer::GetCapabilities().Texture.Max2DSize,
			"Texture dimensions too large: see Renderer::GetCapabilities() for more info");

		m_Spec.Width      = metadata.Width;
		m_Spec.Height     = metadata.Height;
		m_Spec.DataFormat = metadata.DataFormat;
		m_Spec.Levels     = m_Spec.Levels == 0 ? GetMipLevelsForResolution(m_Spec.Width, m_Spec.Height) : m_Spec.Levels;

		m_DataFormat = DataFormatToOpenGLStorageFormat(m_Spec.DataFormat);
		m_InternalFormat = DataFormatToOpenGLInternalFormat(m_Spec.DataFormat);
		QK_CORE_ASSERT(m_InternalFormat & m_DataFormat, "Invalid internal and data format combinaison");

		m_Target = GL_TEXTURE_2D;
		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);
		glPixelStorei(GL_UNPACK_ALIGNMENT, IsPixel4BytesAligned(m_Spec.DataFormat) ? 4 : 1);
		glTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, m_Spec.Width, m_Spec.Height, 0, m_DataFormat,
			DataFormatToOpenGLDataType(m_Spec.DataFormat), *image);

		if (m_Spec.Levels > 1)
		{
			glGenerateMipmap(GL_TEXTURE_2D);
		}

		QK_CORE_ASSERT(glIsTexture(m_RendererID), "Texture is incomplete!");
		QK_DEBUG_CALL(glBindTexture(GL_TEXTURE_2D, 0));
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		QK_PROFILE_FUNCTION();

		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLTexture2D::SetData(const void* data, size_t size)
	{
		QK_PROFILE_FUNCTION();

		size_t pSize = GetPixelFormatSize(m_Spec.DataFormat);
		QK_CORE_ASSERT(size == m_Spec.Width * m_Spec.Height * pSize, "Data must be entire texture");

		glBindTexture(m_Target, m_RendererID);
		glPixelStorei(GL_UNPACK_ALIGNMENT, IsPixel4BytesAligned(m_Spec.DataFormat) ? 4 : 1);
		glTexSubImage2D(m_Target, 0, 0, 0, m_Spec.Width, m_Spec.Height, m_DataFormat,
			DataFormatToOpenGLDataType(m_Spec.DataFormat), data);

		QK_DEBUG_CALL(glBindTexture(m_Target, 0));
	}

	void OpenGLTexture2D::GenerateMipmaps()
	{
		QK_PROFILE_FUNCTION();

		QK_CORE_ASSERT(m_Spec.Levels > 1, "Invalid texture specification for mipmaps");

		glBindTexture(m_Target, m_RendererID);
		glGenerateMipmap(m_Target);

		QK_DEBUG_CALL(glBindTexture(m_Target, 0));
	}

	bool OpenGLTexture2D::operator==(const Texture& other) const
	{
		if (auto* o = dynamic_cast<decltype(this)>(&other))
			return m_RendererID == o->m_RendererID;

		return false;
	}

	///////////////////////////////////////////////////////////////////////////////////
	// OpenGLTexture2DArray
	//

	OpenGLTexture2DArray::OpenGLTexture2DArray(const Texture2DArraySpecification& spec)
		: Texture2DArray(spec)
	{
		QK_PROFILE_FUNCTION();

		QK_CORE_ASSERT(m_Spec.Width <= Renderer::GetCapabilities().Texture.Max2DSize
			&& m_Spec.Height <= Renderer::GetCapabilities().Texture.Max2DSize
			&& m_Spec.Layers <= Renderer::GetCapabilities().Texture.MaxArrayLayers,
			"Texture dimensions too large: see Renderer::GetCapabilities() for more info");

		m_Spec.Levels = m_Spec.Levels == 0 ? GetMipLevelsForResolution(m_Spec.Width, m_Spec.Height) : m_Spec.Levels;

		m_InternalFormat = DataFormatToOpenGLInternalFormat(m_Spec.DataFormat);
		m_DataFormat = DataFormatToOpenGLStorageFormat(m_Spec.DataFormat);
		QK_CORE_ASSERT(m_InternalFormat & m_DataFormat, "Invalid internal and data format combinaison");

		bool multisampled = m_Spec.Samples > 1;
		m_Target = multisampled ? GL_TEXTURE_2D_MULTISAMPLE_ARRAY : GL_TEXTURE_2D_ARRAY;

		glGenTextures(1, &m_RendererID);
		glBindTexture(m_Target, m_RendererID);
		glPixelStorei(GL_UNPACK_ALIGNMENT, IsPixel4BytesAligned(m_Spec.DataFormat) ? 4 : 1);

		if (multisampled)
		{
			QK_CORE_ASSERT(m_Spec.Levels == 1, "Mipmaps are not compatible when multisampling is enabled");
			glTexImage3DMultisample(GL_TEXTURE_2D_MULTISAMPLE_ARRAY, m_Spec.Samples, m_InternalFormat, m_Spec.Width, m_Spec.Height, m_Spec.Layers, GL_FALSE);

			QK_DEBUG_CALL(glBindTexture(GL_TEXTURE_2D_MULTISAMPLE_ARRAY, 0));
		}
		else
		{
			glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, m_InternalFormat, m_Spec.Width, m_Spec.Height, m_Spec.Layers, 0,
				m_DataFormat, DataFormatToOpenGLDataType(m_Spec.DataFormat), NULL);
		}

		QK_CORE_ASSERT(glIsTexture(m_RendererID), "Texture is incomplete!");
		QK_DEBUG_CALL(glBindTexture(m_Target, 0));
	}

	OpenGLTexture2DArray::~OpenGLTexture2DArray()
	{
		QK_PROFILE_FUNCTION();

		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLTexture2DArray::SetData(const void* data, size_t size, uint32_t layer)
	{
		QK_PROFILE_FUNCTION();

		size_t pSize = GetPixelFormatSize(m_Spec.DataFormat);
		QK_CORE_ASSERT(size == m_Spec.Width * m_Spec.Height * pSize, "Data must be entire texture");

		glBindTexture(m_Target, m_RendererID);
		glPixelStorei(GL_UNPACK_ALIGNMENT, IsPixel4BytesAligned(m_Spec.DataFormat) ? 4 : 1);
		glTexSubImage3D(m_Target, 0, 0, 0, layer, m_Spec.Width, m_Spec.Height, 1,
			m_DataFormat, DataFormatToOpenGLDataType(m_Spec.DataFormat), data);

		QK_DEBUG_CALL(glBindTexture(m_Target, 0));
	}

	void OpenGLTexture2DArray::GenerateMipmaps()
	{
		QK_PROFILE_FUNCTION();

		QK_CORE_ASSERT(m_Spec.Levels > 1, "Invalid texture specification for mipmaps");

		glBindTexture(m_Target, m_RendererID);
		glGenerateMipmap(m_Target);

		QK_DEBUG_CALL(glBindTexture(m_Target, 0));
	}

	bool OpenGLTexture2DArray::operator==(const Texture& other) const
	{
		if (auto* o = dynamic_cast<decltype(this)>(&other))
			return m_RendererID == o->m_RendererID;

		return false;
	}
}
