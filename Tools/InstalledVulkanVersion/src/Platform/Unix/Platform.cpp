#include <dlfcn.h>

namespace Platform {

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
