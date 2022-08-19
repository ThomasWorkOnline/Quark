#include "qkpch.h"
#include "UniformBuffer.h"

#include "GraphicsAPI.h"

namespace Quark {

	Scope<UniformBuffer> UniformBuffer::Create(const UniformBufferSpecification& spec)
	{
		return GraphicsAPI::Instance->CreateUniformBuffer(spec);
	}
}
