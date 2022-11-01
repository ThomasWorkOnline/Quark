#include "qkpch.h"

#include <unistd.h>

namespace Quark {

	uint32_t GetCurrentProcessId()
	{
		return static_cast<uint32_t>(getpid());
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
