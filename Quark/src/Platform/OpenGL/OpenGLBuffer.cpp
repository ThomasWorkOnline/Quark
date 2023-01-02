#include "qkpch.h"
#include "OpenGLBuffer.h"

#include <glad/glad.h>

namespace Quark {

	OpenGLVertexBuffer::OpenGLVertexBuffer(size_t size)
		: m_Size(size)
	{
		glGenBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_DYNAMIC_DRAW);

		QK_DEBUG_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(const void* vertices, size_t size)
		: m_Size(size)
	{
		glGenBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);

		QK_DEBUG_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
	}

	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		glDeleteBuffers(1, &m_RendererID);
	}

	void OpenGLVertexBuffer::SetData(const void* vertices, size_t size, size_t offset)
	{
		QK_CORE_ASSERT(size + offset <= m_Size,
			"Written size is too large: Size and Offset parameters must be within the total buffer size");

		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_DYNAMIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, offset, size, vertices);

		QK_DEBUG_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
	}
	
	bool OpenGLVertexBuffer::operator==(const VertexBuffer& other) const
	{
		if (auto* o = dynamic_cast<decltype(this)>(&other))
			return m_RendererID == o->m_RendererID;

		return false;
	}

	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t count, IndexType indexType)
		: IndexBuffer(count, indexType)
	{
		glGenBuffers(1, &m_RendererID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), NULL, GL_DYNAMIC_DRAW);

		QK_DEBUG_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	}

	OpenGLIndexBuffer::OpenGLIndexBuffer(const void* indices, uint32_t count, IndexType indexType)
		: IndexBuffer(count, indexType)
	{
		glGenBuffers(1, &m_RendererID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * IndexDataTypeSize(indexType), indices, GL_STATIC_DRAW);

		QK_DEBUG_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	}

	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		glDeleteBuffers(1, &m_RendererID);
	}

	void OpenGLIndexBuffer::SetData(const void* indices, uint32_t count, uint32_t firstIndex)
	{
		QK_CORE_ASSERT(count + firstIndex <= m_Count,
			"Written size is too large: Count and FirstIndex parameters must be within the total buffer size");

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * IndexDataTypeSize(m_IndexType), NULL, GL_DYNAMIC_DRAW);
		glBufferSubData(
			GL_ELEMENT_ARRAY_BUFFER,
			firstIndex * IndexDataTypeSize(m_IndexType),
			count * IndexDataTypeSize(m_IndexType),
			indices);

		QK_DEBUG_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	}

	bool OpenGLIndexBuffer::operator==(const IndexBuffer& other) const
	{
		if (auto* o = dynamic_cast<decltype(this)>(&other))
			return m_RendererID == o->m_RendererID;

		return false;
	}
}
