#include "Tests/PBRRendering.h"
#include "FontTest/FontTest.h"

namespace Quark {

	int Main(int argc, char** argv)
	{
		auto test = CreateScope<FontTest>();
		test->Run();

		return 0;
	}
}
