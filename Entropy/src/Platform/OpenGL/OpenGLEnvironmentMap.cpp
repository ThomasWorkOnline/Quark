#include "OpenGLEnvironmentMap.h"

namespace Entropy {

	OpenGLEnvironmentMap::OpenGLEnvironmentMap(const std::array<std::string, 6>& filepaths)
		: m_Filepaths(filepaths)
	{
		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);

		int32_t width, height, channels;
		stbi_set_flip_vertically_on_load(false);
		for (size_t i = 0; i < filepaths.size(); i++)
		{
			stbi_uc* data = stbi_load(filepaths[i].c_str(), &width, &height, &channels, 0);
			NT_ASSERT(data, "Failed to load image at path: " << filepaths[i]);

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
			NT_ASSERT(internalFormat & dataFormat, "Image format not supported");

			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, m_InternalFormats[i], width, height, 0, m_DataFormats[i], GL_UNSIGNED_BYTE, data);

			stbi_image_free(data);
		}

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}
	
	OpenGLEnvironmentMap::~OpenGLEnvironmentMap()
	{
		glDeleteTextures(1, &m_RendererID);
	}
	
	void OpenGLEnvironmentMap::Attach(uint32_t textureSlot) const
	{
		glActiveTexture(GL_TEXTURE0 + textureSlot);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);
	}

	void OpenGLEnvironmentMap::Detach() const
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}
}
