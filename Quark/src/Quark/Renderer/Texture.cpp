#include "qkpch.h"
#include "Texture.h"

#include "GraphicsAPI.h"

namespace Quark {

	Texture2D* Texture2D::Create(const Texture2DSpecification& spec)
	{
		return GraphicsAPI::Instance->CreateTexture2D(spec);
	}

	Texture2D* Texture2D::Create(std::string_view filepath, const TextureFormatDescriptor& descriptor)
	{
		return GraphicsAPI::Instance->CreateTexture2D(filepath, descriptor);
	}
}
