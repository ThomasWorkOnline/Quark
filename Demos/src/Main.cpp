#include "Tests/PBRRendering.h"

namespace Quark {

	int Main(int argc, char** argv)
	{
		auto test = CreateScope<PBRRendering>();
		test->Run();

		return 0;
	}
}
