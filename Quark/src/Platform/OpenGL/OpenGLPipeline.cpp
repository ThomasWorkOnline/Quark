#include "qkpch.h"
#include "OpenGLPipeline.h"

#include "OpenGLEnums.h"
#include "OpenGLFont.h"
#include "OpenGLShader.h"
#include "OpenGLTexture.h"

#include <glad/glad.h>

namespace Quark {

	namespace Utils {

		static bool AssureOpenGLTexture(const Texture* texture)
		{
			return dynamic_cast<const OpenGLFont*>(texture)
				|| dynamic_cast<const OpenGLTexture2D*>(texture)
				|| dynamic_cast<const OpenGLTexture2DArray*>(texture);
		}
	}

	OpenGLPipeline::OpenGLPipeline(const PipelineSpecification& spec)
		: Pipeline(spec)
		, m_PrimitiveTopology(PrimitiveTopologyToOpenGL(spec.Topology))
	{
		auto& shaderResources = m_Spec.Shader->GetShaderResources();

		// Shader reflection does not work for shaders compiled from text sources. TODO: fix
#if 0
		QK_CORE_ASSERT(m_Spec.UniformBufferCount == shaderResources.UniformBuffers.size(),
			"Mismatch between shader resources and uniform buffers provided: {0} given but pipeline expected: {1}",
			m_Spec.UniformBufferCount, shaderResources.UniformBuffers.size());
#endif

		glGenVertexArrays(1, &m_RendererID);
		glBindVertexArray(m_RendererID);
	}

	OpenGLPipeline::~OpenGLPipeline()
	{
		glDeleteVertexArrays(1, &m_RendererID);
	}

	void OpenGLPipeline::SetTexture(const Texture* texture, uint32_t textureIndex)
	{
		QK_CORE_ASSERT(Utils::AssureOpenGLTexture(texture), "OpenGLPipeline tried to bind a texture that wasn't created using OpenGL");

		auto glTexture = static_cast<const OpenGLTexture*>(texture->GetHandle());

		glActiveTexture(GL_TEXTURE0 + textureIndex);
		glBindTexture(glTexture->GetTarget(), glTexture->GetRendererID());
	}

	bool OpenGLPipeline::operator==(const Pipeline& other) const
	{
		if (auto* o = dynamic_cast<decltype(this)>(&other))
			return m_RendererID == o->m_RendererID;

		return false;
	}

	void OpenGLPipeline::Bind() const
	{
		// Shader
		{
			auto* glShader = static_cast<const OpenGLShader*>(m_Spec.Shader);
			glUseProgram(glShader->GetRendererID());
		}

		// Uniform buffers
		for (uint32_t i = 0; i < m_Spec.UniformBufferCount; i++)
		{
			auto* uniformBuffer = static_cast<const OpenGLUniformBuffer*>(m_Spec.UniformBuffers[i]);
			glBindBufferBase(GL_UNIFORM_BUFFER, uniformBuffer->GetBinding(), uniformBuffer->GetRendererID());
		}
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
