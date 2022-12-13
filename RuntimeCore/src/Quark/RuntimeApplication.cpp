#include "qkpch.h"
#include "RuntimeApplication.h"

#include <Quark/EntryPoint.h>

namespace Quark {

	int Main(int argc, char** argv)
	{
		RuntimeCore::Init();

		Application* app = nullptr;
		CommandLineArguments args = { argc, argv };

		try
		{
			QK_BEGIN_PROFILE_SESSION("startup.json");
			app = Quark::CreateApplication(args);
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

		RuntimeCore::Shutdown();

		return EXIT_SUCCESS;
	}
}
