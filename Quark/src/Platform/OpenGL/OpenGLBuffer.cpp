#include "qkpch.h"
#include "OpenGLBuffer.h"

#include <glad/glad.h>

namespace Quark {

	OpenGLVertexBuffer::OpenGLVertexBuffer(size_t size)
	{
		QK_PROFILE_FUNCTION();

		glGenBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);

		QK_CORE_TRACE("  Vertex buffer is optimized for GL_DYNAMIC_DRAW");
		QK_DEBUG_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(const void* vertices, size_t size)
	{
		QK_PROFILE_FUNCTION();

		glGenBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);

		QK_CORE_TRACE("  Vertex buffer is optimized for GL_STATIC_DRAW");
		QK_DEBUG_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
	}

	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		QK_PROFILE_FUNCTION();

		glDeleteBuffers(1, &m_RendererID);
	}

	void OpenGLVertexBuffer::SetData(const void* data, size_t size, size_t offset)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);

		QK_DEBUG_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
	}

	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t count)
		: m_Count(count)
	{
		QK_PROFILE_FUNCTION();

		glGenBuffers(1, &m_RendererID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), nullptr, GL_DYNAMIC_DRAW);

		QK_CORE_TRACE("  Index buffer is optimized for GL_DYNAMIC_DRAW");
		QK_DEBUG_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	}

	OpenGLIndexBuffer::OpenGLIndexBuffer(const uint32_t* indices, uint32_t count)
		: m_Count(count)
	{
		QK_PROFILE_FUNCTION();

		glGenBuffers(1, &m_RendererID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);

		QK_CORE_TRACE("  Index buffer is optimized for GL_STATIC_DRAW");
		QK_DEBUG_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	}

	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		QK_PROFILE_FUNCTION();

		glDeleteBuffers(1, &m_RendererID);
	}

	void OpenGLIndexBuffer::SetData(const uint32_t* data, uint32_t count, size_t offset)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset * sizeof(uint32_t), count * sizeof(uint32_t), data);

		QK_DEBUG_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	}
}
