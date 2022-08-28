#include "qkpch.h"
#include "OpenGLPipeline.h"
#include "OpenGLShader.h"

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

		GLenum PrimitiveTopologyToOpenGL(PrimitiveTopology topology)
		{
			switch (topology)
			{
				case PointList:     return GL_POINTS;
				case LineList:      return GL_LINES;
				case LineStrip:     return GL_LINE_STRIP;
				case TriangleList:  return GL_TRIANGLES;
				case TriangleStrip: return GL_TRIANGLE_STRIP;
				case TriangleFan:   return GL_TRIANGLE_FAN;

				QK_ASSERT_NO_DEFAULT("Unknown primitive topology");
			}

			return GL_NONE;
		}
	}

	OpenGLPipeline::OpenGLPipeline(const PipelineSpecification& spec)
		: Pipeline(spec)
		, m_PrimitiveTopology(Utils::PrimitiveTopologyToOpenGL(spec.Topology))
	{
	}

	OpenGLPipeline::~OpenGLPipeline()
	{
	}

	void OpenGLPipeline::BindShader()
	{
		GLuint rendererID = static_cast<OpenGLShader*>(m_Spec.Shader)->GetRendererID();
		glUseProgram(rendererID);
	}

	void OpenGLPipeline::BindVertexAttrib()
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
						Utils::ShaderDataTypeToOpenGLBaseType(element.Type),
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
						Utils::ShaderDataTypeToOpenGLBaseType(element.Type),
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
						Utils::ShaderDataTypeToOpenGLBaseType(element.Type),
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
							Utils::ShaderDataTypeToOpenGLBaseType(element.Type),
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
							Utils::ShaderDataTypeToOpenGLBaseType(element.Type),
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
