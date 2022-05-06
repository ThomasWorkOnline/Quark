#include "qkpch.h"
#include "UniformBuffer.h"

#include "RenderingAPI.h"

// Include all supported API's uniform buffer implementations
#include "Platform/Standalone/OpenGL/OpenGLUniformBuffer.h"

namespace Quark {

	Ref<UniformBuffer> UniformBuffer::Create(size_t size, uint32_t binding)
	{
		switch (RenderingAPI::GetAPI())
		{
			case RenderingAPI::API::OpenGL:
				return CreateRef<OpenGLUniformBuffer>(size, binding);
			default:
				QK_CORE_FATAL("Rendering API not supported");
				return nullptr;
		}
	}
}
