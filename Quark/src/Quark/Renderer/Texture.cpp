#include "qkpch.h"
#include "Texture.h"

#include "GraphicsAPI.h"

namespace Quark {

	Scope<Texture2D> Texture2D::Create(const Texture2DSpecification& spec)
	{
		return GraphicsAPI::Instance->CreateTexture2D(spec);
	}

	Scope<Texture2D> Texture2D::Create(std::string_view filepath, const TextureRenderModes& renderModes)
	{
		return GraphicsAPI::Instance->CreateTexture2D(filepath, renderModes);
	}

	Scope<Texture2DArray> Texture2DArray::Create(const Texture2DArraySpecification& spec)
	{
		return GraphicsAPI::Instance->CreateTexture2DArray(spec);
	}
}
