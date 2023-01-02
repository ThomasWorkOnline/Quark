#include "qkpch.h"
#include "UniformBuffer.h"

#include "GraphicsAPI.cpp"

namespace Quark {

	UniformBuffer::UniformBuffer(size_t size)
		: m_Size(size)
	{
	}

	Scope<UniformBuffer> UniformBuffer::Create(size_t size)
	{
		return s_GraphicsAPI->CreateUniformBuffer(size);
	}
}
