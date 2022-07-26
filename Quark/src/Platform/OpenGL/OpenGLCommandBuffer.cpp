#include "qkpch.h"
#include "OpenGLCommandBuffer.h"

#include "OpenGLBuffer.h"
#include <glad/glad.h>

namespace Quark {

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
