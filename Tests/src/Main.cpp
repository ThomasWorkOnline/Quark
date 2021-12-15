#include "Tests/DeferredDeleteTest.h"
#include "Tests/FontTest.h"
#include "Tests/TextureArrayTest.h"
#include "Tests/Pong.h"
#include "Tests/PBRRendering.h"

int main()
{
	auto test = new TextureArrayTest();
	test->Run();
	delete test;

	return 0;
}
