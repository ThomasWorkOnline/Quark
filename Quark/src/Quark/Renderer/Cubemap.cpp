#include "qkpch.h"
#include "Cubemap.h"

#include "GraphicsAPI.h"

namespace Quark {

	Scope<Cubemap> Cubemap::Create(const CubemapSpecification& spec)
	{
		return GraphicsAPI::Instance->CreateCubemap(spec);
	}
}
