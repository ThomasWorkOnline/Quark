#include "qkpch.h"

#include <dlfcn.h>
#include <unistd.h>
#include <stdio.h>

namespace Quark {

	uint32_t GetCurrentProcessId()
	{
		return static_cast<uint32_t>(getpid());
	}

	size_t GetCurrentProcessMemoryUsage()
	{
		/* BSD, Linux, and OSX */

		struct rusage rusage;
		getrusage(RUSAGE_SELF, &rusage);

		return (size_t)rusage.ru_maxrss;
	}

	void* LoadLibrary(const char* name)
	{
		return dlopen(name, 0);
	}

	void FreeLibrary(void* library)
	{
		dlclose(library);
	}

	void* GetProcAddress(void* library, const char* procName)
	{
		return dlsym(library, procName);
	}
}
