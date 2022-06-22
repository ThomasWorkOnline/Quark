#include "VulkanApp.h"

#include <Quark/EntryPoint.h>

namespace Quark {

	Application* CreateApplication()
	{
		return new VulkanApp();
	}
}
