#include "Tests/PBRRendering.h"

#include <Quark/Core/EntryPoint.h>

namespace Quark {

	Application* CreateApplication()
	{
		return new PBRRendering();
	}
}
