#include "qkpch.h"
#include "OpenGLBuffer.h"

#include <glad/glad.h>

namespace Quark {

	namespace Utils {

		static constexpr GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
		{
			switch (type)
			{
				case ShaderDataType::Float:		return GL_FLOAT;
				case ShaderDataType::Float2:	return GL_FLOAT;
				case ShaderDataType::Float3:	return GL_FLOAT;
				case ShaderDataType::Float4:	return GL_FLOAT;
				case ShaderDataType::Double:	return GL_DOUBLE;
				case ShaderDataType::Double2:	return GL_DOUBLE;
				case ShaderDataType::Double3:	return GL_DOUBLE;
				case ShaderDataType::Double4:	return GL_DOUBLE;
				case ShaderDataType::Mat3:		return GL_FLOAT;
				case ShaderDataType::Mat4:		return GL_FLOAT;
				case ShaderDataType::Mat3d:     return GL_DOUBLE;
				case ShaderDataType::Mat4d:     return GL_DOUBLE;
				case ShaderDataType::Int:		return GL_INT;
				case ShaderDataType::Int2:		return GL_INT;
				case ShaderDataType::Int3:		return GL_INT;
				case ShaderDataType::Int4:		return GL_INT;
				case ShaderDataType::Bool:		return GL_BOOL;

				QK_ASSERT_DEFAULT("Unknown ShaderDataType", GL_NONE);
			}
		}
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(size_t size)
	{
		QK_PROFILE_FUNCTION();

		glGenBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);

		QK_DEBUG_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));

		glGenVertexArrays(1, &m_VAORendererID);
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(const void* vertices, size_t size)
	{
		QK_PROFILE_FUNCTION();

		glGenBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);

		QK_DEBUG_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));

		glGenVertexArrays(1, &m_VAORendererID);
	}

	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		QK_PROFILE_FUNCTION();

		glDeleteBuffers(1, &m_RendererID);
		glDeleteVertexArrays(1, &m_VAORendererID);
	}

	void OpenGLVertexBuffer::Attach() const
	{
		glBindVertexArray(m_VAORendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
	}

	void OpenGLVertexBuffer::Detach() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void OpenGLVertexBuffer::SetData(const void* data, size_t size, size_t offset)
	{
		QK_PROFILE_FUNCTION();

		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);

		QK_DEBUG_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
	}

	void OpenGLVertexBuffer::SetLayout(const BufferLayout& layout)
	{
		m_Layout = layout;

		glBindVertexArray(m_VAORendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);

		QK_CORE_ASSERT(m_Layout.GetCount() > 0, "Layout is empty");

		uint32_t vertexBufferIndex = 0;
		for (const auto& element : m_Layout)
		{
			switch (element.Type)
			{
				case ShaderDataType::Float:
				case ShaderDataType::Float2:
				case ShaderDataType::Float3:
				case ShaderDataType::Float4:
				{
					glEnableVertexAttribArray(vertexBufferIndex);
					glVertexAttribPointer(vertexBufferIndex,
						element.GetComponentCount(),
						Utils::ShaderDataTypeToOpenGLBaseType(element.Type),
						element.Normalized ? GL_TRUE : GL_FALSE,
						(GLsizei)m_Layout.GetStride(),
						(const void*)element.Offset);
					vertexBufferIndex++;
					break;
				}
				case ShaderDataType::Double:
				case ShaderDataType::Double2:
				case ShaderDataType::Double3:
				case ShaderDataType::Double4:
				{
					glEnableVertexAttribArray(vertexBufferIndex);
					glVertexAttribLPointer(vertexBufferIndex,
						element.GetComponentCount(),
						Utils::ShaderDataTypeToOpenGLBaseType(element.Type),
						(GLsizei)m_Layout.GetStride(),
						(const void*)element.Offset);
					vertexBufferIndex++;
					break;
				}
				case ShaderDataType::Int:
				case ShaderDataType::Int2:
				case ShaderDataType::Int3:
				case ShaderDataType::Int4:
				case ShaderDataType::Bool:
				{
					glEnableVertexAttribArray(vertexBufferIndex);
					glVertexAttribIPointer(vertexBufferIndex,
						element.GetComponentCount(),
						Utils::ShaderDataTypeToOpenGLBaseType(element.Type),
						(GLsizei)m_Layout.GetStride(),
						(const void*)element.Offset);
					vertexBufferIndex++;
					break;
				}
				case ShaderDataType::Mat3:
				case ShaderDataType::Mat4:
				{
					uint32_t count = element.GetComponentCount();
					for (uint32_t i = 0; i < count; i++)
					{
						glEnableVertexAttribArray(vertexBufferIndex);
						glVertexAttribPointer(vertexBufferIndex,
							count,
							Utils::ShaderDataTypeToOpenGLBaseType(element.Type),
							element.Normalized ? GL_TRUE : GL_FALSE,
							(GLsizei)m_Layout.GetStride(),
							(const void*)(sizeof(float) * count * i));
						glVertexAttribDivisor(vertexBufferIndex, 1);
						vertexBufferIndex++;
					}
					break;
				}
				case ShaderDataType::Mat3d:
				case ShaderDataType::Mat4d:
				{
					uint32_t count = element.GetComponentCount();
					for (uint32_t i = 0; i < count; i++)
					{
						glEnableVertexAttribArray(vertexBufferIndex);
						glVertexAttribPointer(vertexBufferIndex,
							count,
							Utils::ShaderDataTypeToOpenGLBaseType(element.Type),
							element.Normalized ? GL_TRUE : GL_FALSE,
							(GLsizei)m_Layout.GetStride(),
							(const void*)(sizeof(double) * count * i));
						glVertexAttribDivisor(vertexBufferIndex, 1);
						vertexBufferIndex++;
					}
					break;
				}

				QK_ASSERT_DEFAULT("Unknown ShaderDataType");
			}
		}

		glBindVertexArray(0);
	}

	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t count)
		: m_Count(count)
	{
		QK_PROFILE_FUNCTION();

		glGenBuffers(1, &m_RendererID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), nullptr, GL_DYNAMIC_DRAW);

		QK_DEBUG_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	}

	OpenGLIndexBuffer::OpenGLIndexBuffer(const uint32_t* indices, uint32_t count)
		: m_Count(count)
	{
		QK_PROFILE_FUNCTION();

		glGenBuffers(1, &m_RendererID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);

		QK_DEBUG_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	}

	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		QK_PROFILE_FUNCTION();

		glDeleteBuffers(1, &m_RendererID);
	}

	void OpenGLIndexBuffer::Attach() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
	}

	void OpenGLIndexBuffer::Detach() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void OpenGLIndexBuffer::SetData(const uint32_t* data, uint32_t count, size_t offset)
	{
		QK_PROFILE_FUNCTION();

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset * sizeof(uint32_t), count * sizeof(uint32_t), data);

		QK_DEBUG_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	}
}
