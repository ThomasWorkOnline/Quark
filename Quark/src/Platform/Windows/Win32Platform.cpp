#include "qkpch.h"

#include <Windows.h>

#undef LoadLibrary // From Windows.h

namespace Quark {

	uint32_t GetCurrentProcessId()
	{
		return static_cast<uint32_t>(::GetCurrentProcessId());
	}

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
