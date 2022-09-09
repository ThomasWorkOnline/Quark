#include "qkpch.h"
#include "OpenGLUniformBuffer.h"

#include <glad/glad.h>

namespace Quark {

	OpenGLUniformBuffer::OpenGLUniformBuffer(const UniformBufferSpecification& spec)
		: UniformBuffer(spec)
	{
		QK_PROFILE_FUNCTION();

		glGenBuffers(1, &m_RendererID);
		glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID);
		glBufferData(GL_UNIFORM_BUFFER, spec.Size, nullptr, GL_DYNAMIC_DRAW);

		QK_DEBUG_CALL(glBindBuffer(GL_UNIFORM_BUFFER, 0));
	}

	OpenGLUniformBuffer::~OpenGLUniformBuffer()
	{
		QK_PROFILE_FUNCTION();

		glDeleteBuffers(1, &m_RendererID);
	}

	void OpenGLUniformBuffer::SetData(const void* data, size_t size, size_t offset)
	{
		QK_CORE_ASSERT(size <= m_Spec.Size, "Size parameter must be less than or equal to the total buffer size");

		glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID);
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
