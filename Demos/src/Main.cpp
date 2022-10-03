#include <Quark/EntryPoint.h>

#include "Demos/PBRRenderingDemo.h"
#include "Examples/Example1_BasicRendering.h"
#include "Examples/ModelImporter.h"

#include "FontTest/FontTest.h"
#include "Pong/Pong.h"
#include "TextureArrayTest.h"
#include "VulkanApp.h"

#include <type_traits>

namespace Quark {

	Application* CreateApplication(CommandLineArguments args)
	{
		using App = VulkanApp;

		ApplicationOptions options;
		options.AppName = "Demo Application";
		options.CoreDir = "../Quark";
		options.CommandLineArgs = args;
		options.GraphicsAPI = RHI::OpenGL;
		//options.Flags = ApplicationFlagBits::EnableAudioOutputDevice;

		return new App(options);
	}
}
