#include "qkpch.h"
#include "RuntimeApplication.h"

#include <Quark/EntryPoint.h>

namespace Quark {

	inline bool g_Running = false;

	static void LaunchApplication(CommandLineArguments args)
	{
		Application* app = nullptr;

		try
		{
			QK_BEGIN_PROFILE_SESSION("startup.json");
			app = CreateApplication(args);
			QK_END_PROFILE_SESSION();

			QK_BEGIN_PROFILE_SESSION("runtime.json");
			app->Run();
			QK_END_PROFILE_SESSION();

			QK_BEGIN_PROFILE_SESSION("shutdown.json");
			delete app;
			QK_END_PROFILE_SESSION();
		}
		catch (std::exception& e)
		{
			QK_CORE_ERROR(e.what());

			if (app)
			{
				app->Crash(e);
				delete app;
			}
		}
	}

	int Main(int argc, char** argv)
	{
		RuntimeCore::Init();
		
		CommandLineArguments args = { argc, argv };

		do
		{
			LaunchApplication(args);
		}
		while (g_Running);

		RuntimeCore::Shutdown();

		return EXIT_SUCCESS;
	}
}
