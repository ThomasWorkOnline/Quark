#pragma once

#ifdef _WIN32
#	ifdef _WIN64
		// We're on windows x64
#		define QK_PLATFORM_WINDOWS
#	else
#		error "x86 is not supported"
#	endif
#elif defined(__APPLE__) && defined(__MACH__)
#	include <TargetConditionals.h>

#	if TARGET_IPHONE_SIMULATOR == 1
		// We're on an iPhone Simulator
#		define QK_PLATFORM_IPHONE_SIMULATOR
#		error "iOS simulator is not supported!"
#	elif TARGET_OS_IPHONE == 1
		// We're on iOS
#		define QK_PLATFORM_IOS
#		error "iOS is not supported!"
#	elif TARGET_OS_MAC == 1
		// We're on MacOS
#		define QK_PLATFORM_MACOS
#	else
#		error "Unknown Apple platform!"
#	endif
#elif defined(__ANDROID__)
	// We're on Android
#	define QK_PLATFORM_ANDROID
#	error "Android is not supported"
#elif defined(__linux__)
	// We are on Linux
#	define QK_PLATFORM_LINUX
#else
	#error "Unknown platform is not supported by default"
#endif
