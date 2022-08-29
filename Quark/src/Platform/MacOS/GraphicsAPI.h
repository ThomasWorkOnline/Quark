#pragma once

#include "Quark/Renderer/GraphicsAPI.h"

namespace Quark {

	RHI GetDefaultRHIForPlatform();
	Scope<GraphicsAPI> CreateRHIForPlatform(RHI api);
}
