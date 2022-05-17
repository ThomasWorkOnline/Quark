#include "FontTest/FontTest.h"

#include <Quark/EntryPoint.h>

namespace Quark {

	Application* CreateApplication()
	{
		return new FontTest();
	}
}
