#include "Texture.h"

#include "RenderingAPI.h"

// Include all supported API's textures implementations
#include "../../Platform/OpenGL/OpenGLTexture.h"

namespace Quark {

	Ref<Texture2D> Texture2D::Create(const TextureSpecification& spec)
	{
		switch (RenderingAPI::GetAPI())
		{
		case RenderingAPI::API::OpenGL:
			return CreateRef<OpenGLTexture2D>(spec);
		}

		QK_FATAL("Unknown Rendering API");
		return nullptr;
	}

	Ref<Texture2D> Texture2D::Create(const std::string& filepath)
	{
		switch (RenderingAPI::GetAPI())
		{
		case RenderingAPI::API::OpenGL:
			return CreateRef<OpenGLTexture2D>(filepath);
		}

		QK_FATAL("Unknown Rendering API");
		return nullptr;
	}
}
