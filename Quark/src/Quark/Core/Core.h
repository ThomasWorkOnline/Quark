#pragma once

// Platform detection
#ifdef _WIN32
#	ifdef _WIN64
		// We're on windows x64
#		define QK_PLATFORM_WINDOWS
#		define QK_PLATFORM_NAME "Windows 64-bit"
#	else
#		define QK_PLATFORM_NAME "Windows 32-bit"
#		error "x86 is not supported"
#	endif
#elif defined(__APPLE__) && defined(__MACH__)
#	include <TargetConditionals.h>
	// Base platform for any Apple device
#	define QK_PLATFORM_APPLE

#	if TARGET_IPHONE_SIMULATOR == 1
		// We're on an iPhone Simulator
#		define QK_PLATFORM_IPHONE_SIMULATOR
#		define QK_PLATFORM_NAME "iPhone Simulator"
#		error "iOS simulator is not supported!"
#	elif TARGET_OS_IPHONE == 1
		// We're on iOS
#		define QK_PLATFORM_IOS
#		define QK_PLATFORM_NAME "iOS"
#		error "iOS is not supported!"
#	elif TARGET_OS_MAC == 1
		// We're on MacOS
#		define QK_PLATFORM_MACOS
#		define QK_PLATFORM_NAME "macOS"
#	else
#		error "Unknown Apple platform!"
#	endif
#elif defined(__ANDROID__)
	// We're on Android
#	define QK_PLATFORM_ANDROID
#	define QK_PLATFORM_NAME "Android"
#	error "Android is not supported"
#elif defined(__linux__)
	// We are on Linux
#	define QK_PLATFORM_LINUX
#	define QK_PLATFORM_NAME "Linux"
#else
#	error "Unknown platform"
#endif

#include "Tweaks.h"
#include "Config.h"
#include "Platform.h"
#include "Assertions.h"

#include <utility>

#if __cplusplus >= 202002L
	// C++20 and forward
#	define QK_CONSTEXPR20 constexpr
#else
#	define QK_CONSTEXPR20 inline
#endif

#define QK_NODISCARD [[nodiscard]]

#if QK_IGNORE_DEPRECATED_SYMBOLS
#	define QK_DEPRECATED
#else
#	define QK_DEPRECATED [[deprecated]]	
#endif

#define ATTACH_EVENT_FN(fn) [&](auto&&... args) -> decltype(auto) { return fn(std::forward<decltype(args)>(args)...); }
#define BIT(x) (1 << x)

#include "Timer.h"
#include "Timestep.h"
#include "Memory.h"

// GLM standards configuration
#include "Quark/Math/Types.h"

#include "Quark/Debug/Logger.h"
#include "Quark/Debug/LogUtils.h"
#include "Quark/Debug/Profiling.h"

////////////////////////////////////////////////////////////////
// Logging Macros
// --------------

#define QK_CORE_TRACE(...)    do { QK_CORE_CONSOLE_TRACE(__VA_ARGS__);    QK_DUMP_TRACE(__VA_ARGS__);   } while (false)
#define QK_CORE_INFO(...)     do { QK_CORE_CONSOLE_INFO(__VA_ARGS__);     QK_DUMP_INFO(__VA_ARGS__);    } while (false)
#define QK_CORE_WARN(...)     do { QK_CORE_CONSOLE_WARN(__VA_ARGS__);     QK_DUMP_WARN(__VA_ARGS__);    } while (false)
#define QK_CORE_ERROR(...)    do { QK_CORE_CONSOLE_ERROR(__VA_ARGS__);    QK_DUMP_ERROR(__VA_ARGS__);   } while (false)
#define QK_CORE_CRITICAL(...) do { QK_CORE_CONSOLE_CRITICAL(__VA_ARGS__); QK_DUMP_CRITICAL(__VA_ARGS__);} while (false)

#define QK_TRACE(...)         do { QK_CLIENT_CONSOLE_TRACE(__VA_ARGS__);    QK_DUMP_TRACE(__VA_ARGS__);    } while (false)
#define QK_INFO(...)          do { QK_CLIENT_CONSOLE_INFO(__VA_ARGS__);     QK_DUMP_INFO(__VA_ARGS__);     } while (false)
#define QK_WARN(...)          do { QK_CLIENT_CONSOLE_WARN(__VA_ARGS__);     QK_DUMP_WARN(__VA_ARGS__);     } while (false)
#define QK_ERROR(...)         do { QK_CLIENT_CONSOLE_ERROR(__VA_ARGS__);    QK_DUMP_ERROR(__VA_ARGS__);    } while (false)
#define QK_CRITICAL(...)      do { QK_CLIENT_CONSOLE_CRITICAL(__VA_ARGS__); QK_DUMP_CRITICAL(__VA_ARGS__); } while (false)

namespace Quark {

	namespace RuntimeCore {

		void Init();
		void Shutdown();
	}
}
