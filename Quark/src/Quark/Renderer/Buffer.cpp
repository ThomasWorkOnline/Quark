#include "qkpch.h"
#include "Buffer.h"

#include "RenderCommand.h"

namespace Quark {

	Ref<VertexBuffer> VertexBuffer::Create(const void* vertices, size_t size)
	{
		return RenderCommand::CreateVertexBuffer(vertices, size);
	}

	Ref<VertexBuffer> VertexBuffer::Create(size_t size)
	{
		return RenderCommand::CreateVertexBuffer(size);
	}

	Ref<IndexBuffer> IndexBuffer::Create(const uint32_t* indices, uint32_t count)
	{
		return RenderCommand::CreateIndexBuffer(indices, count);
	}

	Ref<IndexBuffer> IndexBuffer::Create(uint32_t count)
	{
		return RenderCommand::CreateIndexBuffer(count);
	}
}
