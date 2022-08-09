#include "qkpch.h"
#include "OpenGLCommandBuffer.h"

#include "OpenGLBuffer.h"
#include <glad/glad.h>

namespace Quark {

	void OpenGLCommandBuffer::BindPipeline(const Ref<Pipeline>& pipeline)
	{
		pipeline->GetSpecification().Shader->Attach();
	}

	void OpenGLCommandBuffer::BeginRenderPass(const Ref<RenderPass>& renderPass, const Ref<Framebuffer>& framebuffer)
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

	void OpenGLCommandBuffer::BindVertexBuffer(const Ref<VertexBuffer>& vertexBuffer, uint32_t binding)
	{
		static_cast<OpenGLVertexBuffer*>(vertexBuffer.get())->Attach();
	}

	void OpenGLCommandBuffer::BindIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
	{
		static_cast<OpenGLIndexBuffer*>(indexBuffer.get())->Attach();
	}
}
