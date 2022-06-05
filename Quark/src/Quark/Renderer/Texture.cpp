#include "qkpch.h"
#include "Texture.h"

#include "GraphicsAPI.h"

// Include all supported API's texture implementations
#include "Platform/OpenGL/OpenGLTexture.h"

namespace Quark {

	Ref<Texture2D> Texture2D::Create(const Texture2DSpecification& spec)
	{
		switch (GraphicsAPI::GetAPI())
		{
			case GraphicsAPI::API::OpenGL:
				return CreateRef<OpenGLTexture2D>(spec);
			default:
				QK_CORE_FATAL("Graphics API not supported");
				return nullptr;
		}
	}

	Ref<Texture2D> Texture2D::Create(std::string_view filepath, const TextureFormatDescriptor& descriptor)
	{
		switch (GraphicsAPI::GetAPI())
		{
			case GraphicsAPI::API::OpenGL:
				return CreateRef<OpenGLTexture2D>(filepath, descriptor);
			default:
				QK_CORE_FATAL("Graphics API not supported");
				return nullptr;
		}
	}
}
