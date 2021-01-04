#include "Texture.h"

#include "RenderingAPI.h"

// Include all supported API's textures implementations
#include "../../Platform/OpenGL/OpenGLTexture.h"

namespace Entropy {

	Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height)
	{
		switch (RenderingAPI::GetAPI())
		{
		case RenderingAPI::API::OpenGL:
			return CreateRef<OpenGLTexture2D>(width, height);
			break;
		case RenderingAPI::API::None:
			NT_FATAL("Rendering API not supported");
			return nullptr;
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
			break;
		case RenderingAPI::API::None:
			NT_FATAL("Rendering API not supported");
			return nullptr;
		}

		NT_FATAL("Unknown Rendering API");
		return nullptr;
	}
}
