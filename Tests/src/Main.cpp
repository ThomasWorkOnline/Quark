#include "Tests/FontRenderingTest.h"

int main()
{
	auto test = new FontRenderingTest();
	test->Run();
	delete test;

	std::cin.get();

	return 0;
}
