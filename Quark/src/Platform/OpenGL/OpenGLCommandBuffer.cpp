#include "qkpch.h"
#include "OpenGLCommandBuffer.h"

#include "OpenGLBuffer.h"
#include "OpenGLFramebuffer.h"

#include <glad/glad.h>

#define QK_ASSERT_PIPELINE_VALID_STATE(pipeline) QK_CORE_ASSERT(pipeline, "No pipeline was actively bound to the current command buffer!")

namespace Quark {

	void OpenGLCommandBuffer::SetCullFace(RenderCullMode mode)
	{
		switch (mode)
		{
			case RenderCullMode::None:
				glDisable(GL_CULL_FACE);
				break;
			case RenderCullMode::Front:
				glEnable(GL_CULL_FACE);
				glCullFace(GL_FRONT);
				break;
			case RenderCullMode::Back:
				glEnable(GL_CULL_FACE);
				glCullFace(GL_BACK);
				break;
			case RenderCullMode::FrontAndBack:
				glEnable(GL_CULL_FACE);
				glCullFace(GL_FRONT_AND_BACK);
				break;
		}
	}

	void OpenGLCommandBuffer::SetDepthFunction(RenderDepthFunction func)
	{
		switch (func)
		{
			case RenderDepthFunction::Never:
				glDepthFunc(GL_NEVER);
				break;
			case RenderDepthFunction::Always:
				glDepthFunc(GL_ALWAYS);
				break;
			case RenderDepthFunction::NotEqual:
				glDepthFunc(GL_NOTEQUAL);
				break;
			case RenderDepthFunction::Less:
				glDepthFunc(GL_LESS);
				break;
			case RenderDepthFunction::LessEqual:
				glDepthFunc(GL_LEQUAL);
				break;
			case RenderDepthFunction::Greater:
				glDepthFunc(GL_GREATER);
				break;
			case RenderDepthFunction::GreaterEqual:
				glDepthFunc(GL_GEQUAL);
				break;
		}
	}

	void OpenGLCommandBuffer::BindPipeline(Pipeline* pipeline)
	{
		m_BoundPipeline = static_cast<OpenGLPipeline*>(pipeline);
		m_BoundPipeline->Bind();
	}

	void OpenGLCommandBuffer::SetViewport(uint32_t viewportWidth, uint32_t viewportHeight)
	{
		glViewport(0, 0, viewportWidth, viewportHeight);
	}

	void OpenGLCommandBuffer::SetLineWidth(float width)
	{
		glLineWidth(width);
	}

	void OpenGLCommandBuffer::BeginRenderPass(RenderPass* renderPass, Framebuffer* framebuffer)
	{
		QK_ASSERT_PIPELINE_VALID_STATE(m_BoundPipeline);

		framebuffer ? static_cast<OpenGLFramebuffer*>(framebuffer)->Bind() : OpenGLFramebuffer::Bind(0);

		m_CurrentRenderPass = renderPass;
		if (m_CurrentRenderPass->GetSpecification().ClearBuffers)
		{
			auto& color = m_CurrentRenderPass->GetSpecification().ClearColor;
			glClearColor(color.r, color.g, color.b, color.a);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}
	}

	void OpenGLCommandBuffer::EndRenderPass()
	{
		m_CurrentRenderPass = nullptr;
	}

	void OpenGLCommandBuffer::Draw(uint32_t vertexCount, uint32_t vertexOffset)
	{
		QK_ASSERT_PIPELINE_VALID_STATE(m_BoundPipeline);
		glDrawArrays(m_BoundPipeline->GetPrimitiveTopology(), vertexOffset, vertexCount);
	}

	void OpenGLCommandBuffer::DrawIndexed(uint32_t indexCount)
	{
		QK_ASSERT_PIPELINE_VALID_STATE(m_BoundPipeline);
		glDrawElements(m_BoundPipeline->GetPrimitiveTopology(), indexCount, GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLCommandBuffer::DrawInstanced(uint32_t vertexCount, uint32_t vertexOffset, uint32_t instanceCount)
	{
		QK_ASSERT_PIPELINE_VALID_STATE(m_BoundPipeline);
		glDrawArraysInstanced(m_BoundPipeline->GetPrimitiveTopology(), vertexOffset, vertexCount, instanceCount);
	}

	void OpenGLCommandBuffer::DrawIndexedInstanced(uint32_t indexCount, uint32_t instanceCount)
	{
		QK_ASSERT_PIPELINE_VALID_STATE(m_BoundPipeline);
		glDrawElementsInstanced(m_BoundPipeline->GetPrimitiveTopology(), indexCount, GL_UNSIGNED_INT, nullptr, instanceCount);
	}

	void OpenGLCommandBuffer::BindVertexBuffer(VertexBuffer* vertexBuffer)
	{
		QK_ASSERT_PIPELINE_VALID_STATE(m_BoundPipeline);
		QK_CORE_ASSERT(vertexBuffer->GetLayout() == m_BoundPipeline->GetLayout(), "Buffer layout does not match the currently bound pipeline layout");

		GLuint rendererID = static_cast<OpenGLVertexBuffer*>(vertexBuffer)->GetRendererID();
		glBindBuffer(GL_ARRAY_BUFFER, rendererID);
		m_BoundPipeline->BindVertexAttrib();
	}

	void OpenGLCommandBuffer::BindIndexBuffer(IndexBuffer* indexBuffer)
	{
		QK_ASSERT_PIPELINE_VALID_STATE(m_BoundPipeline);

		GLuint rendererID = static_cast<OpenGLIndexBuffer*>(indexBuffer)->GetRendererID();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rendererID);
	}

	bool OpenGLCommandBuffer::IsInsideRenderPass() const
	{
		return m_CurrentRenderPass;
	}
}
