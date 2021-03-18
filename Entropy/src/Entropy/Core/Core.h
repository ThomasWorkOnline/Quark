#pragma once

#include <array>
#include <cstring>
#include <cmath>
#include <fstream>
#include <functional>
#include <future>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

// Platform detection
#include "Platform.h"

// Debugbreak
#if defined(NT_PLATFORM_WINDOWS)
#	define NT_DEBUGBREAK() __debugbreak()
#elif defined(NT_PLATFORM_LINUX)
#	include <signal.h>
#	define NT_DEBUGBREAK() raise(SIGTRAP)
#elif defined(NT_PLATFORM_APPLE)
#	define NT_DEBUGBREAK() abort()
#else
#	define NT_DEBUGBREAK()
#endif

// Configuration
#if defined(_DEBUG)
#	define NT_DEBUG
#else
#	define NT_RELEASE
#endif

// Standards configuration
#define GLM_FORCE_LEFT_HANDED
#include "../../../vendor/glm/glm.hpp"
#include "../../../vendor/glm/gtc/matrix_transform.hpp"
#include "../../../vendor/glm/gtc/quaternion.hpp"
#include "../../../vendor/glm/gtx/quaternion.hpp"

#include "Logger.h"
#include "../Tools/LogUtils.h"

#define NT_FATAL(...) { NT_ERROR(__VA_ARGS__); NT_DEBUGBREAK(); }

#define NT_ASSERT(x, ...) { if(!(x)) { NT_ERROR(__VA_ARGS__); NT_DEBUGBREAK(); } }

#define NT_ATTACH_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

#define BIT(x) (1 << x)

#define NT_FLIP_ON_LOAD 1

namespace Entropy {

	template<typename T>
	using Scope = std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
}
