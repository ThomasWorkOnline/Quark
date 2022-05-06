#include "qkpch.h"
#include "TextureArray.h"

#include "RenderingAPI.h"

// Include all supported API's texture array implementations
#include "Platform/Standalone/OpenGL/OpenGLTextureArray.h"

namespace Quark {

	Ref<Texture2DArray> Texture2DArray::Create(const TextureArraySpecification& spec)
	{
		switch (RenderingAPI::GetAPI())
		{
			case RenderingAPI::API::OpenGL:
				return CreateRef<OpenGLTexture2DArray>(spec);
			default:
				QK_CORE_FATAL("Rendering API not supported");
				return nullptr;
		}
	}
}
