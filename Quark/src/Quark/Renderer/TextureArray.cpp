#include "qkpch.h"
#include "TextureArray.h"

#include "RenderCommand.h"

namespace Quark {

	Ref<Texture2DArray> Texture2DArray::Create(const Texture2DArraySpecification& spec)
	{
		return RenderCommand::CreateTexture2DArray(spec);
	}
}
