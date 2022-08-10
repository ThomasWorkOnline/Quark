#include "qkpch.h"
#include "TextureArray.h"

#include "GraphicsAPI.h"

namespace Quark {

	Texture2DArray* Texture2DArray::Create(const Texture2DArraySpecification& spec)
	{
		return GraphicsAPI::Instance->CreateTexture2DArray(spec);
	}
}
