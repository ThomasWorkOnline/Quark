#pragma once

#include <iostream>

// Use unbuffered std::cerr for errors
#ifdef QK_DEBUG
#	define QK_CORE_TRACE(...) std::cout << "[Trace]: "		<< __VA_ARGS__ << std::endl
#	define QK_CORE_INFO(...)  std::cout << "[Info]: "		<< __VA_ARGS__ << std::endl
#	define QK_CORE_WARN(...)  std::cout << "[Warning]: "	<< __VA_ARGS__ << std::endl
#	define QK_CORE_ERROR(...) std::cerr << "[ERROR]: "		<< __VA_ARGS__ << std::endl
#else
#	define QK_CORE_TRACE(...) // Logging disabled
#	define QK_CORE_INFO(...)  std::cout << "[Info]: "		<< __VA_ARGS__ << std::endl
#	define QK_CORE_WARN(...)  std::cout << "[Warning]: "	<< __VA_ARGS__ << std::endl
#	define QK_CORE_ERROR(...) std::cerr << "[ERROR]: "		<< __VA_ARGS__ << std::endl
#endif
