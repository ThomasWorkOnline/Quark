#include "qkpch.h"
#include "VertexArray.h"

#include "GraphicsAPI.h"

// Include all supported API's vertex arrays implementations
#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace Quark {

	Ref<VertexArray> VertexArray::Create()
	{
		switch (GraphicsAPI::GetAPI())
		{
			case GraphicsAPI::API::OpenGL:
				return CreateRef<OpenGLVertexArray>();
			default:
				QK_CORE_FATAL("Graphics API not supported");
				return nullptr;
		}
	}
}
