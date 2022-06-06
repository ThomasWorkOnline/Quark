#include "qkpch.h"
#include "UniformBuffer.h"

#include "RenderCommand.h"

namespace Quark {

	Ref<UniformBuffer> UniformBuffer::Create(size_t size, uint32_t binding)
	{
		return RenderCommand::CreateUniformBuffer(size, binding);
	}
}
