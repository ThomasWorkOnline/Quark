#include "qkpch.h"
#include "Texture.h"

#include "RenderCommand.h"

namespace Quark {

	Ref<Texture2D> Texture2D::Create(const Texture2DSpecification& spec)
	{
		return RenderCommand::CreateTexture2D(spec);
	}

	Ref<Texture2D> Texture2D::Create(std::string_view filepath, const TextureFormatDescriptor& descriptor)
	{
		return RenderCommand::CreateTexture2D(filepath, descriptor);
	}
}
