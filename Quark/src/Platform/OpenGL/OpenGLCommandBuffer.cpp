#include "qkpch.h"
#include "OpenGLCommandBuffer.h"

#include "OpenGLBuffer.h"
#include <glad/glad.h>

namespace Quark {

	void OpenGLCommandBuffer::BindPipeline(Pipeline* pipeline)
	{
		pipeline->GetSpecification().Shader->Attach();
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
		glDrawArrays(GL_TRIANGLES, vertexOffset, vertexCount);
	}

	void OpenGLCommandBuffer::DrawIndexed(uint32_t indexCount)
	{
		glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLCommandBuffer::DrawIndexedInstanced(uint32_t instanceCount, uint32_t indexCount)
	{
		glDrawElementsInstanced(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr, instanceCount);
	}

	void OpenGLCommandBuffer::DrawLines(uint32_t vertexCount)
	{
		glDrawArrays(GL_LINES, 0, vertexCount);
	}

	void OpenGLCommandBuffer::BindVertexBuffer(VertexBuffer* vertexBuffer, uint32_t binding)
	{
		static_cast<OpenGLVertexBuffer*>(vertexBuffer)->Attach();
	}

	void OpenGLCommandBuffer::BindIndexBuffer(IndexBuffer* indexBuffer)
	{
		static_cast<OpenGLIndexBuffer*>(indexBuffer)->Attach();
	}
}
