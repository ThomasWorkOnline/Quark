#include "qkpch.h"
#include "OpenGLCommandBuffer.h"

#include "OpenGLBuffer.h"
#include "OpenGLShader.h"

#include <glad/glad.h>

namespace Quark {

	void OpenGLCommandBuffer::BindPipeline(Pipeline* pipeline)
	{
		m_BoundPipeline = reinterpret_cast<OpenGLPipeline*>(pipeline);
		GLuint rendererID = reinterpret_cast<OpenGLShader*>(pipeline->GetSpecification().Shader)->GetRendererID();
		glUseProgram(rendererID);
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
			glClearColor(0.01f, 0.01f, 0.01f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}
	}

	void OpenGLCommandBuffer::EndRenderPass()
	{
	}

	void OpenGLCommandBuffer::Draw(uint32_t vertexOffset, uint32_t vertexCount)
	{
		GLenum type = m_BoundPipeline->GetPrimitiveTopologyState();
		glDrawArrays(type, vertexOffset, vertexCount);
	}

	void OpenGLCommandBuffer::DrawIndexed(uint32_t indexCount)
	{
		GLenum type = m_BoundPipeline->GetPrimitiveTopologyState();
		glDrawElements(type, indexCount, GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLCommandBuffer::DrawIndexedInstanced(uint32_t instanceCount, uint32_t indexCount)
	{
		GLenum type = m_BoundPipeline->GetPrimitiveTopologyState();
		glDrawElementsInstanced(type, indexCount, GL_UNSIGNED_INT, nullptr, instanceCount);
	}

	void OpenGLCommandBuffer::DrawLines(uint32_t vertexCount)
	{
		// TODO: set pipeline topology state to GL_LINES
		glDrawArrays(GL_LINES, 0, vertexCount);
	}

	void OpenGLCommandBuffer::BindVertexBuffer(VertexBuffer* vertexBuffer)
	{
		GLuint vaoRendererID = static_cast<OpenGLVertexBuffer*>(vertexBuffer)->GetVAORendererID();
		glBindVertexArray(vaoRendererID);
	}

	void OpenGLCommandBuffer::BindIndexBuffer(IndexBuffer* indexBuffer)
	{
		GLuint rendererID = static_cast<OpenGLIndexBuffer*>(indexBuffer)->GetRendererID();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rendererID);
	}
}
