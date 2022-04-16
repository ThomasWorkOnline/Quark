#include "Tests/PBRRendering.h"

namespace Demos {

	int Main(int argc, char** argv)
	{
		auto test = Quark::CreateScope<PBRRendering>();
		test->Run();

		return 0;
	}
}

#if defined(QK_PLATFORM_WINDOWS) && defined(QK_DIST)
#include <Windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	return Demos::Main(__argc, __argv);
}

#else

int main(int argc, char** argv)
{
	return Demos::Main(argc, argv);
}

#endif /* QK_PLATFORM_WINDOWS */
