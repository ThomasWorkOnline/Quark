#include "Tests/DeferredDeleteTest.h"
#include "Tests/FontTest.h"

int main()
{
	auto test = new FontTest();
	test->Run();
	delete test;

	return 0;
}
