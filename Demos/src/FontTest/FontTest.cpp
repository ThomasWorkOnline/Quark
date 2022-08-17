#include "FontTest.h"

#include "MainLayer.h"

FontTest::FontTest(const ApplicationOptions& options)
	: Application(options)
{
	PushLayer(new MainLayer(this));
}
