#include "qkpch.h"
#include "Buffer.h"

#include "GraphicsAPI.h"

namespace Quark {

	Ref<VertexBuffer> VertexBuffer::Create(const void* vertices, size_t size)
	{
		return GraphicsAPI::Instance->CreateVertexBuffer(vertices, size);
	}

	Ref<VertexBuffer> VertexBuffer::Create(size_t size)
	{
		return GraphicsAPI::Instance->CreateVertexBuffer(size);
	}

	Ref<IndexBuffer> IndexBuffer::Create(const uint32_t* indices, uint32_t count)
	{
		return GraphicsAPI::Instance->CreateIndexBuffer(indices, count);
	}

	Ref<IndexBuffer> IndexBuffer::Create(uint32_t count)
	{
		return GraphicsAPI::Instance->CreateIndexBuffer(count);
	}
}
