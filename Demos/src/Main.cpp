#include <Quark/EntryPoint.h>

#include "Demos/PBRRenderingDemo.h"
#include "Examples/Example1_BasicRendering.h"
#include "Examples/ModelImporter.h"

#include "FontTest/FontTest.h"
#include "Pong/Pong.h"
#include "TextureArrayTest.h"
#include "VulkanApp.h"

namespace Quark {

	Application* CreateApplication(CommandLineArguments args)
	{
		using App = VulkanApp;

		ApplicationSpecification appSpec;
		appSpec.Samples = 4;
		appSpec.AppName = "Demo Application";
		appSpec.CoreDir = "../Quark";
		appSpec.CommandLineArgs = args;
		appSpec.GraphicsAPI = RHI::OpenGL;
		appSpec.Flags = ApplicationFlagBits::LaunchRenderer2D;

		return new App(appSpec);
	}
}
