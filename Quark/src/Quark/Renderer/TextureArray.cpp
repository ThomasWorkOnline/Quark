#include "TextureArray.h"

#include "RenderingAPI.h"

// Include all supported API's texture array implementations
#include "Platform/OpenGL/OpenGLTextureArray.h"

namespace Quark {

	Ref<Texture2DArray> Texture2DArray::Create(const TextureArraySpecification& spec)
	{
		switch (RenderingAPI::GetAPI())
		{
			case RenderingAPI::API::OpenGL:
				return CreateRef<OpenGLTexture2DArray>(spec);
			default:
				QK_FATAL("Rendering API not supported");
				return nullptr;
		}
	}
}
