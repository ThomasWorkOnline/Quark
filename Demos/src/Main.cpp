#include "Tests/PBRRendering.h"

int main()
{
	auto test = Quark::CreateScope<PBRRendering>();
	test->Run();
}
