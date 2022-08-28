#include <Quark/EntryPoint.h>

#include "Demos/PBRRenderingDemo.h"
#include "FontTest/FontTest.h"
#include "Example1_BasicRendering.h"
#include "TextureArrayTest.h"
#include "VulkanApp.h"

namespace Quark {

	Application* CreateApplication()
	{
		using App = FontTest;

		ApplicationOptions options;
		options.AppName = "Demo Application";
		options.CoreAssetDir = "../";

		return new App(options);
	}
}
