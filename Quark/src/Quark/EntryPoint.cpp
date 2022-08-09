#include "qkpch.h"
#include "EntryPoint.h"

namespace Quark {

	static int Main(int argc, char** argv)
	{
		RuntimeCore::Init();
		Application* app = nullptr;

		try
		{
			QK_BEGIN_PROFILE_SESSION("startup.json");
			app = Quark::CreateApplication();
			QK_END_PROFILE_SESSION();

			QK_BEGIN_PROFILE_SESSION("runtime.json");
			app->Run();
			QK_END_PROFILE_SESSION();
		}
		catch (std::exception& e)
		{
			if (app) app->OnCrash();
			QK_CORE_ASSERT(false, e.what());
		}

		QK_BEGIN_PROFILE_SESSION("shutdown.json");
		delete app;
		QK_END_PROFILE_SESSION();

		RuntimeCore::Shutdown();
		return 0;
	}
}

#if defined(QK_PLATFORM_WINDOWS) && defined(QK_DIST)
#include <Windows.h>

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	return Quark::Main(__argc, __argv);
}

#else

int main(int argc, char** argv)
{
	return Quark::Main(argc, argv);
}

#endif /* QK_PLATFORM_WINDOWS */
