#include "qkpch.h"
#include "OpenGLVertexArray.h"

#include <glad/glad.h>

namespace Quark {

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
			case ShaderDataType::Mat3f:		return GL_FLOAT;
			case ShaderDataType::Mat4f:		return GL_FLOAT;
			case ShaderDataType::Mat3d:     return GL_DOUBLE;
			case ShaderDataType::Mat4d:     return GL_DOUBLE;
			case ShaderDataType::Int:		return GL_INT;
			case ShaderDataType::Int2:		return GL_INT;
			case ShaderDataType::Int3:		return GL_INT;
			case ShaderDataType::Int4:		return GL_INT;
			case ShaderDataType::Bool:		return GL_BOOL;

			QK_ASSERT_NO_DEFAULT("Unknown ShaderDataType");
		}

		return GL_NONE;
	}

	OpenGLVertexArray::OpenGLVertexArray()
	{
		QK_PROFILE_FUNCTION();

		glGenVertexArrays(1, &m_RendererID);
	}

	OpenGLVertexArray::~OpenGLVertexArray()
	{
		glDeleteVertexArrays(1, &m_RendererID);
	}

	void OpenGLVertexArray::Attach() const
	{
		glBindVertexArray(m_RendererID);
	}

	void OpenGLVertexArray::Detach() const
	{
		glBindVertexArray(0);
	}

	void OpenGLVertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
	{
		QK_PROFILE_FUNCTION();

		QK_CORE_ASSERT(vertexBuffer, "Vertex buffer is empty");
		QK_CORE_ASSERT(m_Layout.GetCount() != 0, "Vertex array has no layout");

		glBindVertexArray(m_RendererID);
		vertexBuffer->Attach();

		GLuint vertexBufferIndex = 0;
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
						ShaderDataTypeToOpenGLBaseType(element.Type),
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
						ShaderDataTypeToOpenGLBaseType(element.Type),
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
						ShaderDataTypeToOpenGLBaseType(element.Type),
						(GLsizei)m_Layout.GetStride(),
						(const void*)element.Offset);
					vertexBufferIndex++;
					break;
				}
				case ShaderDataType::Mat3f:
				case ShaderDataType::Mat4f:
				{
					uint32_t count = element.GetComponentCount();
					for (uint32_t i = 0; i < count; i++)
					{
						glEnableVertexAttribArray(vertexBufferIndex);
						glVertexAttribPointer(vertexBufferIndex,
							count,
							ShaderDataTypeToOpenGLBaseType(element.Type),
							element.Normalized ? GL_TRUE : GL_FALSE,
							(GLsizei)m_Layout.GetStride(),
							(const void*)(sizeof(float) * count * i));
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
							ShaderDataTypeToOpenGLBaseType(element.Type),
							element.Normalized ? GL_TRUE : GL_FALSE,
							(GLsizei)m_Layout.GetStride(),
							(const void*)(sizeof(double) * count * i));
						vertexBufferIndex++;
					}
					break;
				}

				QK_ASSERT_NO_DEFAULT("Unknown ShaderDataType");
			}
		}

		glBindVertexArray(0);
		m_VertexBuffers.push_back(vertexBuffer);
	}

	void OpenGLVertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
	{
		QK_CORE_ASSERT(indexBuffer, "Index Buffer is empty");

		indexBuffer->Attach();
		m_IndexBuffer = indexBuffer;
	}
}
