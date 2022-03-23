#pragma once

#include "Core.h"

#include <iostream>

// Use unbuffered std::cerr for errors
#ifdef QK_DEBUG
#	define QK_CORE_TRACE(...) std::cout << "[Core Trace]: "     << __VA_ARGS__ << std::endl
#	define QK_CORE_INFO(...)  std::cout << "[Core Info]: "      << __VA_ARGS__ << std::endl
#	define QK_CORE_WARN(...)  std::cout << "[Core Warning]: "   << __VA_ARGS__ << std::endl
#	define QK_CORE_ERROR(...) std::cerr << "[CORE ERROR]: "     << __VA_ARGS__ << std::endl

#	define QK_TRACE(...) std::cout << "[Trace]: "               << __VA_ARGS__ << std::endl
#	define QK_INFO(...)  std::cout << "[Info]: "                << __VA_ARGS__ << std::endl
#	define QK_WARN(...)  std::cout << "[Warning]: "             << __VA_ARGS__ << std::endl
#	define QK_ERROR(...) std::cerr << "[ERROR]: "               << __VA_ARGS__ << std::endl
#else
#	define QK_CORE_TRACE(...)                                   // Tracing disabled
#	define QK_CORE_INFO(...)  std::cout << "[Core Info]: "      << __VA_ARGS__ << std::endl
#	define QK_CORE_WARN(...)  std::cout << "[Core Warning]: "   << __VA_ARGS__ << std::endl
#	define QK_CORE_ERROR(...) std::cerr << "[CORE ERROR]: "     << __VA_ARGS__ << std::endl

#	define QK_TRACE(...)                                        // Tracing disabled
#	define QK_INFO(...)  std::cout << "[Info]: "                << __VA_ARGS__ << std::endl
#	define QK_WARN(...)  std::cout << "[Warning]: "             << __VA_ARGS__ << std::endl
#	define QK_ERROR(...) std::cerr << "[ERROR]: "               << __VA_ARGS__ << std::endl
#endif

#include "LogUtils.h"
