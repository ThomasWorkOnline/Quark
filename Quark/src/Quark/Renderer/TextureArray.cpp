#include "qkpch.h"
#include "TextureArray.h"

#include "GraphicsAPI.h"

// Include all supported API's texture array implementations
#include "Platform/OpenGL/OpenGLTextureArray.h"

namespace Quark {

	Ref<Texture2DArray> Texture2DArray::Create(const TextureArraySpecification& spec)
	{
		switch (GraphicsAPI::GetAPI())
		{
			case GraphicsAPI::API::OpenGL:
				return CreateRef<OpenGLTexture2DArray>(spec);
			default:
				QK_CORE_FATAL("Graphics API not supported");
				return nullptr;
		}
	}
}
