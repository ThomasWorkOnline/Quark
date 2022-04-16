#include "Tests/PBRRendering.h"

namespace Quark {

	int Main(int argc, char** argv)
	{
		auto test = Quark::CreateScope<PBRRendering>();
		test->Run();

		return 0;
	}
}
