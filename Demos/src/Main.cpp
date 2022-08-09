#include "VulkanApp.h"
#include "Demos/PBRRenderingDemo.h"
#include "TextureArrayTest.h"

#include <Quark/EntryPoint.h>

namespace Quark {

	Application* CreateApplication()
	{
		return new TextureArrayTest();
	}
}
