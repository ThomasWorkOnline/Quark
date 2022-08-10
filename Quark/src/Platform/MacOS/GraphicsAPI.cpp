#include "qkpch.h"
#include "GraphicsAPI.h"

namespace Quark {

	RHI GetDefaultAPIForPlatform()
	{
		return RHI::Metal;
	}
}
