#pragma once

#include <cstdint>

namespace Quark {

	extern uint32_t GetCurrentProcessId();
	extern size_t   GetCurrentProcessMemoryUsage();

	extern void*    LoadLibrary(const char* name);
	extern void     FreeLibrary(void* library);

	extern void*    GetProcAddress(void* library, const char* procName);

	template<typename Proc>
	inline Proc     GetProcAddress(void* library, const char* procName)
	{
		return (Proc)GetProcAddress(library, procName);
	}
}
