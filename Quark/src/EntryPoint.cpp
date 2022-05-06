#include "qkpch.h"
#include "Quark/Core/Core.h"
#include "Quark/Core/Application.h"

namespace Quark {

	static int Main(int argc, char** argv)
	{
		QK_BEGIN_PROFILE_SESSION("startup.json");
		auto app = Quark::CreateApplication();
		QK_END_PROFILE_SESSION();

		QK_BEGIN_PROFILE_SESSION("runtime.json");
		app->Run();
		QK_END_PROFILE_SESSION();

		QK_BEGIN_PROFILE_SESSION("shutdown.json");
		delete app;
		QK_END_PROFILE_SESSION();

		return 0;
	}
}

#if defined(QK_PLATFORM_WINDOWS) && defined(QK_DIST)
#include <Windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	return Quark::Main(__argc, __argv);
}

#else

int main(int argc, char** argv)
{
	return Quark::Main(argc, argv);
}

#endif /* QK_PLATFORM_WINDOWS */
