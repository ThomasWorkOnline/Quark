#include "Texture.h"

#include "RenderingAPI.h"

// Include all supported API's texture implementations
#include "Platform/OpenGL/OpenGLTexture.h"

namespace Quark {

	Ref<Texture2D> Texture2D::Create(const TextureSpecification& spec)
	{
		switch (RenderingAPI::GetAPI())
		{
			case RenderingAPI::API::OpenGL:
				return CreateRef<OpenGLTexture2D>(spec);
			default:
				QK_FATAL("Rendering API not supported");
				return nullptr;
		}
	}

	Ref<Texture2D> Texture2D::Create(std::string_view filepath, const TextureRenderModes& modes)
	{
		switch (RenderingAPI::GetAPI())
		{
			case RenderingAPI::API::OpenGL:
				return CreateRef<OpenGLTexture2D>(filepath, modes);
			default:
				QK_FATAL("Rendering API not supported");
				return nullptr;
		}
	}
}
