#include "qkpch.h"
#include "OpenGLCommandBuffer.h"

#include "OpenGLBuffer.h"
#include "OpenGLUniformBuffer.h"

#include <glad/glad.h>

namespace Quark {

	void OpenGLCommandBuffer::BindPipeline(Pipeline* pipeline)
	{
		m_Pipeline = static_cast<OpenGLPipeline*>(pipeline);
		m_Pipeline->BindShader();

		for (auto* uniformBuffer : pipeline->GetSpecification().UniformBuffers)
		{
			auto* ub = static_cast<OpenGLUniformBuffer*>(uniformBuffer);
			glBindBufferBase(GL_UNIFORM_BUFFER, ub->GetBinding(), ub->GetRendererID());
		}
	}

	void OpenGLCommandBuffer::SetViewport(uint32_t viewportWidth, uint32_t viewportHeight)
	{
		glViewport(0, 0, viewportWidth, viewportHeight);
	}

	void OpenGLCommandBuffer::BeginRenderPass(RenderPass* renderPass, Framebuffer* framebuffer)
	{
		framebuffer ? framebuffer->Attach() : glBindFramebuffer(GL_FRAMEBUFFER, 0);

		if (renderPass->GetSpecification().ClearBuffers)
		{
			auto& color = renderPass->GetSpecification().ClearColor;
			glClearColor(color.r, color.g, color.b, color.a);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}
	}

	void OpenGLCommandBuffer::EndRenderPass()
	{
	}

	void OpenGLCommandBuffer::Draw(uint32_t vertexCount, uint32_t vertexOffset)
	{
		glDrawArrays(m_Pipeline->GetPrimitiveTopology(), vertexOffset, vertexCount);
	}

	void OpenGLCommandBuffer::DrawIndexed(uint32_t indexCount)
	{
		glDrawElements(m_Pipeline->GetPrimitiveTopology(), indexCount, GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLCommandBuffer::DrawInstanced(uint32_t vertexCount, uint32_t vertexOffset, uint32_t instanceCount)
	{
		glDrawArraysInstanced(m_Pipeline->GetPrimitiveTopology(), vertexOffset, vertexCount, instanceCount);
	}

	void OpenGLCommandBuffer::DrawIndexedInstanced(uint32_t indexCount, uint32_t instanceCount)
	{
		glDrawElementsInstanced(m_Pipeline->GetPrimitiveTopology(), indexCount, GL_UNSIGNED_INT, nullptr, instanceCount);
	}

	void OpenGLCommandBuffer::BindVertexBuffer(VertexBuffer* vertexBuffer)
	{
		QK_CORE_ASSERT(vertexBuffer->GetLayout() == m_Pipeline->GetLayout(), "Buffer layout does not match the currently bound pipeline layout");

		GLuint rendererID = static_cast<OpenGLVertexBuffer*>(vertexBuffer)->GetRendererID();
		glBindBuffer(GL_ARRAY_BUFFER, rendererID);
		m_Pipeline->BindVertexAttrib();
	}

	void OpenGLCommandBuffer::BindIndexBuffer(IndexBuffer* indexBuffer)
	{
		GLuint rendererID = static_cast<OpenGLIndexBuffer*>(indexBuffer)->GetRendererID();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rendererID);
	}
}
