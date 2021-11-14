#include "Tests/DeferredDeleteTest.h"
#include "Tests/FontTest.h"
#include "Tests/TextureArrayTest.h"
#include "Tests/Pong.h"

int main()
{
	auto test = new FontTest();
	test->Run();
	delete test;

	return 0;
}
