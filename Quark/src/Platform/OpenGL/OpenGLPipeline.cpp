#include "qkpch.h"
#include "OpenGLPipeline.h"

#include "OpenGLEnums.h"

#include <glad/glad.h>

namespace Quark {

	OpenGLPipeline::OpenGLPipeline(const PipelineSpecification& spec)
		: Pipeline(spec)
		, m_PrimitiveTopology(PrimitiveTopologyToOpenGL(spec.Topology))
	{
		glGenVertexArrays(1, &m_RendererID);
		glBindVertexArray(m_RendererID);
	}

	OpenGLPipeline::~OpenGLPipeline()
	{
		glDeleteVertexArrays(1, &m_RendererID);
	}

	bool OpenGLPipeline::operator==(const Pipeline& other) const
	{
		if (auto* o = dynamic_cast<decltype(this)>(&other))
			return m_RendererID == o->m_RendererID;

		return false;
	}

	void OpenGLPipeline::BindVertexAttrib() const
	{
		GLuint vertexBufferIndex = 0;
		for (const auto& element : m_Spec.Layout)
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
						element.Normalized,
						(GLsizei)m_Spec.Layout.GetStride(),
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
						(GLsizei)m_Spec.Layout.GetStride(),
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
						(GLsizei)m_Spec.Layout.GetStride(),
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
							element.Normalized,
							(GLsizei)m_Spec.Layout.GetStride(),
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
							element.Normalized,
							(GLsizei)m_Spec.Layout.GetStride(),
							(const void*)(sizeof(double) * count * i));
						vertexBufferIndex++;
					}
					break;
				}

				QK_ASSERT_NO_DEFAULT("Unknown ShaderDataType");
			}
		}
	}
}
