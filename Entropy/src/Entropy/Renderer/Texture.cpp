#include "Texture.h"

#include "RenderingAPI.h"

// Include all supported API's textures implementations
#include "../../Platform/OpenGL/OpenGLTexture.h"

namespace Entropy {

	Ref<Texture2D> Texture2D::Create(const TextureSpecification& spec)
	{
		switch (RenderingAPI::GetAPI())
		{
		case RenderingAPI::API::OpenGL:
			return CreateRef<OpenGLTexture2D>(spec);
		}

		NT_FATAL("Unknown Rendering API");
		return nullptr;
	}

	Ref<Texture2D> Texture2D::Create(const std::string& filepath)
	{
		switch (RenderingAPI::GetAPI())
		{
		case RenderingAPI::API::OpenGL:
			return CreateRef<OpenGLTexture2D>(filepath);
		}

		NT_FATAL("Unknown Rendering API");
		return nullptr;
	}
}
