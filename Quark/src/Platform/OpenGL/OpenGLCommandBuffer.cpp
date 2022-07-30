#include "qkpch.h"
#include "OpenGLCommandBuffer.h"

#include "OpenGLBuffer.h"
#include <glad/glad.h>

namespace Quark {

	void OpenGLCommandBuffer::BeginRenderPass(const Ref<RenderPass>& renderPass, const Ref<Framebuffer>& framebuffer)
	{
		if (renderPass->GetSpecification().Clears)
		{
			glClearColor(0.01f, 0.01f, 0.01f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}
	}

	void OpenGLCommandBuffer::EndRenderPass()
	{
	}

	void OpenGLCommandBuffer::DrawIndexed(uint32_t indexCount)
	{
		glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
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
