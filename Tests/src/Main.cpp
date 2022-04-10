#include "Pong/Pong.h"

int main()
{
	auto test = Quark::CreateScope<Pong>();
	test->Run();
}
