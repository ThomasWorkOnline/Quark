#include "FontTest.h"

#include "MainLayer.h"

FontTest::FontTest()
{
	PushLayer(new MainLayer(this));
}
