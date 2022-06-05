#include "qkpch.h"
#include "UniformBuffer.h"

#include "GraphicsAPI.h"

// Include all supported API's uniform buffer implementations
#include "Platform/OpenGL/OpenGLUniformBuffer.h"

namespace Quark {

	Ref<UniformBuffer> UniformBuffer::Create(size_t size, uint32_t binding)
	{
		switch (GraphicsAPI::GetAPI())
		{
			case GraphicsAPI::API::OpenGL:
				return CreateRef<OpenGLUniformBuffer>(size, binding);
			default:
				QK_CORE_FATAL("Graphics API not supported");
				return nullptr;
		}
	}
}
