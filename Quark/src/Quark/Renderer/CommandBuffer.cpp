#include "qkpch.h"
#include "CommandBuffer.h"

#include "GraphicsAPI.h"

namespace Quark {

	Ref<CommandBuffer> CommandBuffer::Create()
	{
		return GraphicsAPI::Instance->CreateCommandBuffer();
	}
}
