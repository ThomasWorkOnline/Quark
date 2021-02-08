#pragma once

#ifdef _WIN32
#	ifdef _WIN64
		// We're on windows x64
#		define NT_PLATFORM_WINDOWS
#	else
		// We're on windows x86
#		error "x86 is not supported"
#	endif
#elif defined(__APPLE__) && defined(__MACH__)
	// We're on Macos X
#	include <TargetConditionals.h>
#	define NT_PLATFORM_APPLE
	// We are on Macos
#elif defined(__ANDROID__)
	// We're on Android
#	define NT_PLATFORM_ANDROID
#	error "Android is not supported"
#elif defined(__linux__)
	// We're on Linux
#	define NT_PLATFORM_LINUX
	// We are on Linux
#else
	// Unknown platform
	#error "Unknown platform is not supported by default"
#endif
