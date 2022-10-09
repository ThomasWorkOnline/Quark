#include <Windows.h>

#undef LoadLibrary

namespace Platform {

	void* LoadLibrary(const char* name)
	{
		return ::LoadLibraryA((LPCSTR)name);
	}

	void FreeLibrary(void* library)
	{
		::FreeLibrary((HMODULE)library);
	}

	void* GetProcAddress(void* library, const char* procName)
	{
		return ::GetProcAddress((HMODULE)library, (LPCSTR)procName);
	}
}
