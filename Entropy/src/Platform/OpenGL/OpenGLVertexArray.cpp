#include "OpenGLVertexArray.h"

#include "../../Entropy/Core/Core.h"

#include <GL/glew.h>

namespace Entropy {

	// type conversion table
	static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::Float:    return GL_FLOAT;
		case ShaderDataType::Float2:   return GL_FLOAT;
		case ShaderDataType::Float3:   return GL_FLOAT;
		case ShaderDataType::Float4:   return GL_FLOAT;
		case ShaderDataType::Mat3:     return GL_FLOAT;
		case ShaderDataType::Mat4:     return GL_FLOAT;
		case ShaderDataType::Int:      return GL_INT;
		case ShaderDataType::Int2:     return GL_INT;
		case ShaderDataType::Int3:     return GL_INT;
		case ShaderDataType::Int4:     return GL_INT;
		case ShaderDataType::Bool:     return GL_BOOL;
		}

		NT_FATAL("Unknown ShaderDataType");

		return 0;
	}

	OpenGLVertexArray::OpenGLVertexArray()
	{
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

	void OpenGLVertexArray::AddVertexBuffer(Ref<VertexBuffer> vertexBuffer)
	{
		NT_ASSERT(vertexBuffer->GetLayout().GetElements().size() != 0, "Vertex buffer has no layout");

		glBindVertexArray(m_RendererID);

		NT_ASSERT(vertexBuffer != nullptr, "Vertex Buffer is empty");
		vertexBuffer->Attach();

		const auto& layout = vertexBuffer->GetLayout();
		for (const auto& element : layout)
		{
			switch (element.Type)
			{
			case ShaderDataType::Float:
			case ShaderDataType::Float2:
			case ShaderDataType::Float3:
			case ShaderDataType::Float4:
			case ShaderDataType::Int:
			case ShaderDataType::Int2:
			case ShaderDataType::Int3:
			case ShaderDataType::Int4:
			case ShaderDataType::Bool:
			{
				glEnableVertexAttribArray(m_VertexBufferIndex);
				glVertexAttribPointer(m_VertexBufferIndex,
					element.GetComponentCount(),
					ShaderDataTypeToOpenGLBaseType(element.Type),
					element.Normalized ? GL_TRUE : GL_FALSE,
					(GLsizei)layout.GetStride(),
					(const void*)element.Offset);
				m_VertexBufferIndex++;

				break;
			}
			case ShaderDataType::Mat3:
			case ShaderDataType::Mat4:
			{
				uint32_t count = element.GetComponentCount();
				for (uint32_t i = 0; i < count; i++)
				{
					glEnableVertexAttribArray(m_VertexBufferIndex);
					glVertexAttribPointer(m_VertexBufferIndex,
						count,
						ShaderDataTypeToOpenGLBaseType(element.Type),
						element.Normalized ? GL_TRUE : GL_FALSE,
						(GLsizei)layout.GetStride(),
						(const void*)(sizeof(float) * count * i));
					glVertexAttribDivisor(m_VertexBufferIndex, 1);
					m_VertexBufferIndex++;
				}

				break;
			}
			default:
				NT_FATAL("Unknown ShaderDataType");
			}
		}

		m_VertexBuffers.push_back(vertexBuffer);
	}

	void OpenGLVertexArray::SetIndexBuffer(Ref<IndexBuffer> indexBuffer)
	{
		glBindVertexArray(m_RendererID);

		NT_ASSERT(indexBuffer != nullptr, "Index Buffer is empty");
		indexBuffer->Attach();

		m_IndexBuffer = indexBuffer;
	}
}
