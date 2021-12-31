#include "Tests/DeferredDeleteTest.h"
#include "Tests/FontTest.h"
#include "Tests/PBRRendering.h"
#include "Tests/Pong.h"
#include "Tests/TextureArrayTest.h"

int main()
{
	auto test = new TextureArrayTest();
	test->Run();
	delete test;

	return 0;
}
