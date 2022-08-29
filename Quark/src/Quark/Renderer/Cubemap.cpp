#include "qkpch.h"
#include "Cubemap.h"

#include "GraphicsAPI.cpp"

namespace Quark {

	Scope<Cubemap> Cubemap::Create(const CubemapSpecification& spec)
	{
		return s_GraphicsAPI->CreateCubemap(spec);
	}
}
