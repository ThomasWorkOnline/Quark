#pragma once

#include "Tweaks.h"

// Platform detection
#ifdef _WIN32

	#ifdef _WIN64
		// We're on windows x64
		#define QK_PLATFORM_WINDOWS
		#define QK_PLATFORM_NAME "Windows 64-bit"
	#else
		#define QK_PLATFORM_NAME "Windows 32-bit"
		#error "x86 is not supported"
	#endif

#elif defined(__APPLE__) && defined(__MACH__)

	#include <TargetConditionals.h>
	// Base platform for any Apple device
	#define QK_PLATFORM_APPLE

	#if TARGET_IPHONE_SIMULATOR == 1
		// We're on an iPhone Simulator
		#define QK_PLATFORM_IPHONE_SIMULATOR
		#define QK_PLATFORM_NAME "iPhone Simulator"
		#error "iOS simulator is not supported!"
	#elif TARGET_OS_IPHONE == 1
		// We're on iOS
		#define QK_PLATFORM_IOS
		#define QK_PLATFORM_NAME "iOS"
		#error "iOS is not supported!"
	#elif TARGET_OS_MAC == 1
		// We're on MacOS
		#define QK_PLATFORM_MACOS
		#define QK_PLATFORM_NAME "macOS"
	#else
		#error "Unknown Apple platform!"
	#endif

#elif defined(__ANDROID__)

	// We're on Android
	#define QK_PLATFORM_ANDROID
	#define QK_PLATFORM_NAME "Android"
	#error "Android is not supported"

#elif defined(__linux__)

	// We are on Linux
	#define QK_PLATFORM_LINUX
	#define QK_PLATFORM_NAME "Linux"

#else
	#error "Unknown platform"
#endif

#define QK_NODISCARD [[nodiscard]]

#if QK_IGNORE_DEPRECATED_SYMBOLS
	#define QK_DEPRECATED
#else
	#define QK_DEPRECATED [[deprecated]]	
#endif

#if defined(QK_DEBUG)
	#define QK_CONFIG_NAME "Debug"
#elif defined(QK_RELEASE)
	#define QK_CONFIG_NAME "Release"
#elif defined(QK_DIST)
	#define QK_CONFIG_NAME "Dist"
#else
	#error "Unknown configuration"
#endif

#ifdef QK_DEBUG
	#define QK_DEBUG_CALL(x) x
#else
	#define QK_DEBUG_CALL(x)
#endif

#ifdef QK_DEBUG
	#define QK_ENABLE_ASSERTIONS 1
#else
	#define QK_ENABLE_ASSERTIONS 0
#endif

#include "Assertions.h"
#include "Memory.h"