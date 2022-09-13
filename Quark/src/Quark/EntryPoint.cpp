#include "qkpch.h"
#include "EntryPoint.h"

namespace Quark {

	static int Main(int argc, char** argv)
	{
		RuntimeCore::Init();

		Scope<Application> app;

		try
		{
			CommandLineArguments args = { argc, argv };

			QK_BEGIN_PROFILE_SESSION("startup.json");
			app.reset(Quark::CreateApplication(args));
			QK_END_PROFILE_SESSION();

			QK_BEGIN_PROFILE_SESSION("runtime.json");
			app->Run();
			QK_END_PROFILE_SESSION();

			QK_BEGIN_PROFILE_SESSION("shutdown.json");
			app.reset();
			QK_END_PROFILE_SESSION();
		}
		catch (std::exception& e)
		{
			if (app)
			{
				app->OnCrash();
			}

			(void)e;
			QK_CORE_ASSERT(false, e.what());
		}

		RuntimeCore::Shutdown();

		return EXIT_SUCCESS;
	}
}

#if defined(QK_PLATFORM_WINDOWS) && defined(QK_DIST)
#include <Windows.h>

INT WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ INT nShowCmd)
{
	return Quark::Main(__argc, __argv);
}

#else

int main(int argc, char** argv)
{
	return Quark::Main(argc, argv);
}

#endif /* QK_PLATFORM_WINDOWS */
