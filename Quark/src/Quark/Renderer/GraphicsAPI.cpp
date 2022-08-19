#include "qkpch.h"
#include "GraphicsAPI.h"

namespace Quark {

	RHI GraphicsAPI::s_API = RHI::None;
	Scope<GraphicsAPI> GraphicsAPI::Instance = Instantiate();

	Scope<GraphicsAPI> GraphicsAPI::Instantiate()
	{
		s_API = GetDefaultRHIForPlatform();
		return CreateDefaultRHIForPlatform();
	}
}
