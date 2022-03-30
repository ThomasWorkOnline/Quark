#include "OpenGLCubeMap.h"

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <glad/glad.h>

namespace Quark {

	OpenGLCubeMap::OpenGLCubeMap(const std::array<std::string_view, 6>& filepaths)
	{
		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);

		int32_t width, height, channels;
		stbi_set_flip_vertically_on_load(false);
		for (size_t i = 0; i < filepaths.size(); i++)
		{
			stbi_uc* data = stbi_load(filepaths[i].data(), &width, &height, &channels, 0);
			QK_CORE_ASSERT(data, "Failed to load image at path: {0}", filepaths[i]);

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

			m_InternalFormats[i] = internalFormat;
			m_DataFormats[i] = dataFormat;
			QK_CORE_ASSERT(internalFormat & dataFormat, "Image format not supported");

			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);

			stbi_image_free(data);
		}

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 0);
	}
	
	OpenGLCubeMap::~OpenGLCubeMap()
	{
		glDeleteTextures(1, &m_RendererID);
	}
	
	void OpenGLCubeMap::Attach(uint32_t textureSlot) const
	{
		glActiveTexture(GL_TEXTURE0 + textureSlot);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);
	}

	void OpenGLCubeMap::Detach() const
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}
}
