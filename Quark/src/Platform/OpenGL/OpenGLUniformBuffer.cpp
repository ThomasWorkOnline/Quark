#include "qkpch.h"
#include "OpenGLUniformBuffer.h"

#include "Quark/Renderer/Renderer.h"

#include <glad/glad.h>

namespace Quark {

	OpenGLUniformBuffer::OpenGLUniformBuffer(size_t size)
		: UniformBuffer(size)
	{
		QK_CORE_ASSERT(m_Size <= Renderer::GetCapabilities().UniformBuffer.MaxBufferSize,
			"Uniform buffer Size too large: see Renderer::GetCapabilities() for more info");

		glGenBuffers(1, &m_RendererID);
		glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID);
		glBufferData(GL_UNIFORM_BUFFER, m_Size, NULL, GL_DYNAMIC_DRAW);

		QK_DEBUG_CALL(glBindBuffer(GL_UNIFORM_BUFFER, 0));
	}

	OpenGLUniformBuffer::~OpenGLUniformBuffer()
	{
		glDeleteBuffers(1, &m_RendererID);
	}

	void OpenGLUniformBuffer::SetData(const void* data, size_t size, size_t offset)
	{
		QK_CORE_ASSERT(size + offset <= m_Size,
			"Written size is too large: Size and Offset parameters must be within the total buffer size");

		glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID);
		glBufferData(GL_UNIFORM_BUFFER, size, NULL, GL_DYNAMIC_DRAW);
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);

		QK_DEBUG_CALL(glBindBuffer(GL_UNIFORM_BUFFER, 0));
	}

	bool OpenGLUniformBuffer::operator==(const UniformBuffer& other) const
	{
		if (auto* o = dynamic_cast<decltype(this)>(&other))
			return m_RendererID == o->m_RendererID;

		return false;
	}
}
