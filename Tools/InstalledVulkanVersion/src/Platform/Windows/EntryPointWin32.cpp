#include "Platform.h"

#include <Windows.h>

extern int Main(int argc, char** argv);

INT WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ INT nShowCmd)
{
	return Main(__argc, __argv);
}
