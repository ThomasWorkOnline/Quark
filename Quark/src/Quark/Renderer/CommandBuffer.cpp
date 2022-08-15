#include "qkpch.h"
#include "CommandBuffer.h"

#include "GraphicsAPI.h"

namespace Quark {

	Scope<CommandBuffer> CommandBuffer::Create()
	{
		return GraphicsAPI::Instance->CreateCommandBuffer();
	}
}
