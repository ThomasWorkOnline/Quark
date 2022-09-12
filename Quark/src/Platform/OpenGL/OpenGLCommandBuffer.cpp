#include "qkpch.h"
#include "OpenGLCommandBuffer.h"

#include "OpenGLBuffer.h"
#include "OpenGLEnums.h"
#include "OpenGLFramebuffer.h"

#include <glad/glad.h>

#define QK_ASSERT_PIPELINE_VALID_STATE(pipeline) QK_CORE_ASSERT(pipeline, "No pipeline was actively bound to the current command buffer!")

namespace Quark {

	void OpenGLCommandBuffer::SetCullFace(RenderCullMode mode)
	{
		mode == RenderCullMode::None
			? glDisable(GL_CULL_FACE)
			: glEnable(GL_CULL_FACE);

		glCullFace(CullModeToOpenGL(mode));
	}

	void OpenGLCommandBuffer::SetDepthFunction(RenderDepthFunction func)
	{
		glDepthFunc(DepthFunctionToOpenGL(func));
	}

	void OpenGLCommandBuffer::BindPipeline(const Pipeline* pipeline)
	{
		m_BoundPipeline = static_cast<const OpenGLPipeline*>(pipeline);
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

	void OpenGLCommandBuffer::BeginRenderPass(const RenderPass* renderPass, const Framebuffer* framebuffer)
	{
		framebuffer ? static_cast<const OpenGLFramebuffer*>(framebuffer)->Bind() : OpenGLFramebuffer::Bind(0);

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

	void OpenGLCommandBuffer::BindVertexBuffer(const VertexBuffer* vertexBuffer)
	{
		QK_ASSERT_PIPELINE_VALID_STATE(m_BoundPipeline);
		QK_CORE_ASSERT(vertexBuffer->GetLayout() == m_BoundPipeline->GetLayout(), "Buffer layout does not match the currently bound pipeline layout");

		auto* glVertexBuffer = static_cast<const OpenGLVertexBuffer*>(vertexBuffer);

		glBindBuffer(GL_ARRAY_BUFFER, glVertexBuffer->GetRendererID());
		m_BoundPipeline->BindVertexAttrib();
	}

	void OpenGLCommandBuffer::BindIndexBuffer(const IndexBuffer* indexBuffer)
	{
		QK_ASSERT_PIPELINE_VALID_STATE(m_BoundPipeline);

		auto* glIndexBuffer = static_cast<const OpenGLIndexBuffer*>(indexBuffer);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glIndexBuffer->GetRendererID());
	}

	bool OpenGLCommandBuffer::IsInsideRenderPass() const
	{
		return m_CurrentRenderPass;
	}

	bool OpenGLCommandBuffer::operator==(const CommandBuffer& other) const
	{
		// Returns true only if other is an OpenGLCommandBuffer
		return dynamic_cast<decltype(this)>(&other);
	}
}
