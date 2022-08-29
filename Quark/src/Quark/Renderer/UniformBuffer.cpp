#include "qkpch.h"
#include "UniformBuffer.h"

#include "GraphicsAPI.cpp"

namespace Quark {

	Scope<UniformBuffer> UniformBuffer::Create(const UniformBufferSpecification& spec)
	{
		return s_GraphicsAPI->CreateUniformBuffer(spec);
	}
}
