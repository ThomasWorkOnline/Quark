#include "Texture.h"

#include "RenderingAPI.h"

// Include all supported API's textures implementations
#include "../../Platform/OpenGL/OpenGLTexture.h"

#include "../Core/Application.h"

namespace Quark {

	Ref<Texture2D> Texture2D::Create(const TextureSpecification& spec)
	{
		Ref<Texture2D> ref;

		switch (RenderingAPI::GetAPI())
		{
		case RenderingAPI::API::OpenGL:
			ref = CreateRef<OpenGLTexture2D>(spec);
			break;
		case RenderingAPI::API::None:
			QK_FATAL("Rendering API not supported");
		}

		Application::Get().GetResourceManager().Hold(ref);
		return ref;
	}

	Ref<Texture2D> Texture2D::Create(const std::string& filepath)
	{
		Ref<Texture2D> ref;

		switch (RenderingAPI::GetAPI())
		{
		case RenderingAPI::API::OpenGL:
			ref = CreateRef<OpenGLTexture2D>(filepath);
			break;
		case RenderingAPI::API::None:
			QK_FATAL("Rendering API not supported");
		}

		Application::Get().GetResourceManager().Hold(ref);
		return ref;
	}
}
