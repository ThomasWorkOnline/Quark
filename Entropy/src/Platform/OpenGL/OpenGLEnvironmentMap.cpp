#include "OpenGLEnvironmentMap.h"

namespace Entropy {

	OpenGLEnvironmentMap::OpenGLEnvironmentMap(const std::array<std::string, 6>& filepaths)
		: m_Filepaths(filepaths)
	{
		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);
		stbi_set_flip_vertically_on_load(false);

		// TODO: do the same as the framebuffer
		m_InternalFormat = GL_RGB8;
		m_DataFormat = GL_RGB;

		int width, height, channels;
		for (size_t i = 0; i < filepaths.size(); i++)
		{
			unsigned char* data = stbi_load(filepaths[i].c_str(), &width, &height, &channels, 0);
			if (data)
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, m_InternalFormat, width, height, 0, m_DataFormat, GL_UNSIGNED_BYTE, data);
			else
				NT_ERROR("Cubemap tex failed to load at path: " << filepaths[i]);

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
		glCullFace(GL_BACK);
		glDepthFunc(GL_LEQUAL);
		glActiveTexture(GL_TEXTURE0 + textureSlot);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);
	}

	void OpenGLEnvironmentMap::Detach() const
	{
		glCullFace(GL_FRONT);
		glDepthFunc(GL_LESS);

		for (int i = 0; i < 32; i++)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
		}
	}
}
