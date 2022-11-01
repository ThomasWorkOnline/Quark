#include "qkpch.h"

#include "Quark/EntryPoint.h"

#if defined(QK_DIST)

#include <Windows.h>

INT WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ INT nShowCmd)
{
	return Quark::Main(__argc, __argv);
}

#else

int main(int argc, char** argv)
{
	return Quark::Main(argc, argv);
}

#endif /* QK_DIST */
