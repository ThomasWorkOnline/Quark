#pragma once

#include <Quark.h>

#include "MainLayer.h"

class FontTest : public Quark::Application
{
public:
	FontTest()
	{
		PushLayer(new MainLayer());
	}
};
