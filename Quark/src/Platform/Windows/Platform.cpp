#include "qkpch.h"

#include <Windows.h>

namespace Quark {

	uint32_t GetCurrentProcessId()
	{
		return ::GetCurrentProcessId();
	}
}
