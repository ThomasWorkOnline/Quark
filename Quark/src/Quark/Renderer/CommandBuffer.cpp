#include "qkpch.h"
#include "CommandBuffer.h"

#include "GraphicsAPI.h"

namespace Quark {

	CommandBuffer* CommandBuffer::Create()
	{
		return GraphicsAPI::Instance->CreateCommandBuffer();
	}
}
