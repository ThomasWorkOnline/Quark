#pragma once

#include "Application.h"

#ifdef NT_PLATFORM_WINDOWS
#	include <Windows.h>
#endif

extern Entropy::Application* Entropy::CreateApplication();

int main()
{
#if defined(NT_PLATFORM_WINDOWS) &! defined (_DEBUG)
	FreeConsole();
#endif

	/* application creation */
	auto app = Entropy::CreateApplication();
	app->Run();
	delete app;

	return 0;
}
