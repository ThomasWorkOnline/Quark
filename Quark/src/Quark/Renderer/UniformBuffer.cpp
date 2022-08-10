#include "qkpch.h"
#include "UniformBuffer.h"

#include "GraphicsAPI.h"

namespace Quark {

	UniformBuffer* UniformBuffer::Create(size_t size, uint32_t binding)
	{
		return GraphicsAPI::Instance->CreateUniformBuffer(size, binding);
	}
}
