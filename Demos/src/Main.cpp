#include <Quark/EntryPoint.h>

#include "Demos/PBRRenderingDemo.h"
#include "FontTest/FontTest.h"
#include "Example1_BasicRendering.h"
#include "TextureArrayTest.h"
#include "VulkanApp.h"

namespace Quark {

	Application* CreateApplication()
	{
		using App = VulkanApp;

		ApplicationOptions options;
		options.AppName = "Demo Application";
		options.CoreAssetDir = "../";
		options.GraphicsAPI = RHI::Vulkan;

		return new App(options);
	}
}
