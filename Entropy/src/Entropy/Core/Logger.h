#pragma once

#include <iostream>

#ifdef NT_DEBUG
#define NT_TRACE(...) std::cout << __VA_ARGS__ << std::endl
#define NT_INFO(...)  std::cout << __VA_ARGS__ << std::endl
#define NT_WARN(...)  std::cout << __VA_ARGS__ << std::endl
#define NT_ERROR(...) std::cout << __VA_ARGS__ << std::endl
#else
#define NT_TRACE(...) // Logging disabled
#define NT_INFO(...)  std::cout << __VA_ARGS__ << std::endl
#define NT_WARN(...)  std::cout << __VA_ARGS__ << std::endl
#define NT_ERROR(...) std::cout << __VA_ARGS__ << std::endl
#endif
