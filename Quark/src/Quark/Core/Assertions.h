#pragma once

#include "Core.h"

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
#	elif defined(QK_PLATFORM_MACOS)
#		define QK_DEBUGBREAK() abort()
#	else
#		error "Missing debugbreak implementation for platform"
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

#define QK_ASSERT_NO_DEFAULT(msg, optReturnValue) default: QK_CORE_ASSERT(false, msg); return optReturnValue

#define QK_RUNTIME_VERIFY(x, msg) do { if(!(x)) { QK_CORE_ERROR(msg); QK_DEBUGBREAK(); throw std::exception(msg); } } while (false)
