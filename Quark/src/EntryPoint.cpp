#include "Quark/Core/Application.h"

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
