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
#	error "Apple platform is currently not supported (support will come soon!)"
#elif defined(__ANDROID__)
	// We're on Android
#	define NT_PLATFORM_ANDROID
#	error "Android is not supported"
#elif defined(__linux__)
	// We're on Linux
#	define NT_PLATFORM_LINUX
#	error "Linux is not currently supported (support will come soon!)"
#else
	// Unknown platform
	#error "Unknown platform is not supported by default"
#endif
