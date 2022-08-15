#include "qkpch.h"
#include "OpenGLUniformBuffer.h"

#include <glad/glad.h>

namespace Quark {

	OpenGLUniformBuffer::OpenGLUniformBuffer(size_t size, uint32_t binding)
	{
		QK_PROFILE_FUNCTION();

		glGenBuffers(1, &m_RendererID);
		glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID);
		glBindBufferBase(GL_UNIFORM_BUFFER, binding, m_RendererID);
		glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);

		QK_DEBUG_CALL(glBindBuffer(GL_UNIFORM_BUFFER, 0));
	}

	OpenGLUniformBuffer::~OpenGLUniformBuffer()
	{
		QK_PROFILE_FUNCTION();

		glDeleteBuffers(1, &m_RendererID);
	}

	void OpenGLUniformBuffer::SetData(const void* data, size_t size, size_t offset)
	{
		glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID);
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);

		QK_DEBUG_CALL(glBindBuffer(GL_UNIFORM_BUFFER, 0));
	}
}
