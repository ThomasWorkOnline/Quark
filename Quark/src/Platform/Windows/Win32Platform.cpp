#include "qkpch.h"

#include <Windows.h>
#include <Psapi.h>

#undef LoadLibrary // From Windows.h

namespace Quark {

	uint32_t GetCurrentProcessId()
	{
		return static_cast<uint32_t>(::GetCurrentProcessId());
	}

	size_t GetCurrentProcessMemoryUsage()
	{
		HANDLE hProcess = GetCurrentProcess();

		PROCESS_MEMORY_COUNTERS processMemory{};
		processMemory.cb = sizeof(processMemory);
		GetProcessMemoryInfo(hProcess, &processMemory, sizeof(processMemory));

		return processMemory.PeakWorkingSetSize;
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
