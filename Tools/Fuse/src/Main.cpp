#include <Quark/EntryPoint.h>

#include "FuseEditor.h"

namespace Quark {

	Application* CreateApplication(CommandLineArguments args)
	{
		ApplicationOptions options;
		options.Width = 1280;
		options.Height = 720;
		options.AppName = "Fuse Editor";
		options.CoreDir = "../../Quark";
		options.CommandLineArgs = args;
		options.GraphicsAPI = RHI::OpenGL;

		return new FuseEditor(options);
	}
}
