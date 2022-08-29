#include "qkpch.h"
#include "Texture.h"

#include "GraphicsAPI.cpp"

namespace Quark {

	Scope<Texture2D> Texture2D::Create(const Texture2DSpecification& spec)
	{
		return s_GraphicsAPI->CreateTexture2D(spec);
	}

	Scope<Texture2D> Texture2D::Create(std::string_view filepath, const TextureRenderModes& renderModes)
	{
		return s_GraphicsAPI->CreateTexture2D(filepath, renderModes);
	}

	Scope<Texture2DArray> Texture2DArray::Create(const Texture2DArraySpecification& spec)
	{
		return s_GraphicsAPI->CreateTexture2DArray(spec);
	}
}
