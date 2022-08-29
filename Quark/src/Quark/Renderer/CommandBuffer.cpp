#include "qkpch.h"
#include "CommandBuffer.h"

#include "GraphicsAPI.cpp"

namespace Quark {

	Scope<CommandBuffer> CommandBuffer::Create()
	{
		return s_GraphicsAPI->CreateCommandBuffer();
	}
}
