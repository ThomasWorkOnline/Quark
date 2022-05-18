#include "Tests/PBRRendering.h"

#include <Quark/EntryPoint.h>

namespace Quark {

	Application* CreateApplication()
	{
		return new PBRRendering();
	}
}
