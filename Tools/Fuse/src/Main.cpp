#include <Quark/EntryPoint.h>

#include "FuseEditor.h"

namespace Quark {

	Application* CreateApplication(CommandLineArguments args)
	{
		ApplicationSpecification appSpec;
		appSpec.Width = 1280;
		appSpec.Height = 720;
		appSpec.AppName = "Fuse Editor";
		appSpec.CoreDir = "../../Quark";
		appSpec.CommandLineArgs = args;
		appSpec.GraphicsAPI = RHI::OpenGL;

		return new FuseEditor(appSpec);
	}
}
