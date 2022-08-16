#include "qkpch.h"
#include "OpenGLUniformBuffer.h"

#include <glad/glad.h>

namespace Quark {

	OpenGLUniformBuffer::OpenGLUniformBuffer(size_t size, uint32_t binding) : UniformBuffer(size),
		m_Binding(binding)
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
		QK_CORE_ASSERT(size <= m_Size, "Size parameter must be less than or equal to the total buffer size");

		glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID);
		glBindBufferBase(GL_UNIFORM_BUFFER, m_Binding, m_RendererID);
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);

		QK_DEBUG_CALL(glBindBuffer(GL_UNIFORM_BUFFER, 0));
	}
}
