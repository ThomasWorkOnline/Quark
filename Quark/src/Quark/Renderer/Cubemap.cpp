#include "qkpch.h"
#include "Cubemap.h"

#include "GraphicsAPI.h"

namespace Quark {

	Ref<Cubemap> Cubemap::Create(const CubemapSpecification& spec)
	{
		return GraphicsAPI::Instance->CreateCubemap(spec);
	}
}
