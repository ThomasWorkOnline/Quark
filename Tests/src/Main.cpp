#include "Tests/DeferredDeleteTest.h"

int main()
{
	auto test = new DeferredDeleteTest();
	test->Run();
	delete test;

	std::cin.get();

	return 0;
}
