#pragma once

#include "Application.h"

extern Entropy::Application* Entropy::CreateApplication();

int main()
{
	/* application creation */
	auto app = Entropy::CreateApplication();
	app->Run();
	delete app;

	return 0;
}
