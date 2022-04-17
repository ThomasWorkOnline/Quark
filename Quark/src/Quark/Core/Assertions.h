#pragma once

#include "Core.h"
#include "Logger.h"

#if defined(QK_DEBUG)
#   define QK_ENABLE_ASSERTIONS
#endif

// Debugbreak
#ifdef QK_ENABLE_ASSERTIONS
#	if defined(QK_PLATFORM_WINDOWS)
#		define QK_DEBUGBREAK() __debugbreak()
#	elif defined(QK_PLATFORM_LINUX)
#		include <signal.h>
#		define QK_DEBUGBREAK() raise(SIGTRAP)
#	elif defined(QK_PLATFORM_APPLE)
#		define QK_DEBUGBREAK() abort()
#	else
		// Missing debugbreak implementation for platform
#		define QK_DEBUGBREAK()
#	endif
#else
#	define QK_DEBUGBREAK()
#endif

#ifdef QK_ENABLE_ASSERTIONS
#	define QK_CORE_ASSERT(x, ...) do { if(!(x)) { QK_CORE_CRITICAL(__VA_ARGS__); QK_DEBUGBREAK(); } } while (false)
#	define QK_ASSERT(x, ...)      do { if(!(x)) { QK_CRITICAL(__VA_ARGS__);      QK_DEBUGBREAK(); } } while (false)
#else
#	define QK_CORE_ASSERT(x, ...)
#	define QK_ASSERT(x, ...)
#endif

#define QK_CORE_FATAL(...) do { QK_CORE_CRITICAL(__VA_ARGS__); QK_DEBUGBREAK(); } while (false)
