#include "qkpch.h"
#include "Cubemap.h"

#include "RenderCommand.h"

namespace Quark {

	Ref<Cubemap> Cubemap::Create(const CubemapSpecification& spec)
	{
		return RenderCommand::CreateCubemap(spec);
	}
}
