#include <Quark/EntryPoint.h>

#include "Demos/PBRRenderingDemo.h"
#include "FontTest/FontTest.h"
#include "Pong/Pong.h"
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
		options.GraphicsAPI = RHI::OpenGL;
		//options.Flags = ApplicationFlagBits::EnableAudioOutputDevice;

		return new App(options);
	}
}
