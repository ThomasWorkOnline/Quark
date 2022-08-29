#include "qkpch.h"
#include "Buffer.h"

#include "GraphicsAPI.cpp"

namespace Quark {

	Scope<VertexBuffer> VertexBuffer::Create(const void* vertices, size_t size)
	{
		return s_GraphicsAPI->CreateVertexBuffer(vertices, size);
	}

	Scope<VertexBuffer> VertexBuffer::Create(size_t size)
	{
		return s_GraphicsAPI->CreateVertexBuffer(size);
	}

	Scope<IndexBuffer> IndexBuffer::Create(const uint32_t* indices, uint32_t count)
	{
		return s_GraphicsAPI->CreateIndexBuffer(indices, count);
	}

	Scope<IndexBuffer> IndexBuffer::Create(uint32_t count)
	{
		return s_GraphicsAPI->CreateIndexBuffer(count);
	}
}
