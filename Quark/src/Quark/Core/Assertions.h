#pragma once

#if defined(QK_DEBUG)
#   define QK_ENABLE_ASSERTIONS
#endif

#include <format>
#include <stdexcept>

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
#	define QK_CORE_ASSERT_SILENT(x, ...) do { if (!(x)) { QK_DEBUGBREAK(); } } while (false)
#	define QK_CORE_ASSERT(x, ...)   do { if (!(x)) { QK_CORE_CRITICAL(__VA_ARGS__); QK_DEBUGBREAK(); } } while (false)
#	define QK_ASSERT(x, ...)        do { if (!(x)) { QK_CRITICAL(__VA_ARGS__);      QK_DEBUGBREAK(); } } while (false)
#else
#	define QK_CORE_ASSERT_SILENT(x, ...)
#	define QK_CORE_ASSERT(x, ...)
#	define QK_ASSERT(x, ...)
#endif

#define QK_ASSERT_NO_DEFAULT(msg) default: { QK_CORE_ASSERT(false, msg); } break

#define ThrowRuntimeError(...) throw std::runtime_error(std::format(__VA_ARGS__))
#define Verify(x, ...) do { if (!(x)) { ThrowRuntimeError(__VA_ARGS__); } } while (false)
