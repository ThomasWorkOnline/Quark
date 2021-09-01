#pragma once

#include <iostream>

#ifdef QK_DEBUG
#	define QK_CORE_TRACE(...) std::cout << __VA_ARGS__ << std::endl
#	define QK_CORE_INFO(...)  std::cout << __VA_ARGS__ << std::endl
#	define QK_CORE_WARN(...)  std::cout << __VA_ARGS__ << std::endl
#	define QK_CORE_ERROR(...) std::cout << __VA_ARGS__ << std::endl
#else
#	define QK_CORE_TRACE(...) // Logging disabled
#	define QK_CORE_INFO(...)  std::cout << __VA_ARGS__ << std::endl
#	define QK_CORE_WARN(...)  std::cout << __VA_ARGS__ << std::endl
#	define QK_CORE_ERROR(...) std::cout << __VA_ARGS__ << std::endl
#endif
