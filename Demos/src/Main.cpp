#include "VulkanApp.h"
#include "Demos/PBRRenderingDemo.h"

#include <Quark/EntryPoint.h>

namespace Quark {

	Application* CreateApplication()
	{
		return new VulkanApp();
	}
}
