#include "FontTest.h"

#include "MainLayer.h"

FontTest::FontTest(const ApplicationSpecification& spec)
	: Application(spec)
{
	PushLayer(new MainLayer(this));
}
