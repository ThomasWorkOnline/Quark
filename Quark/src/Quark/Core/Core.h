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
#if defined(QK_PLATFORM_WINDOWS)
#	define QK_DEBUGBREAK() __debugbreak()
#elif defined(QK_PLATFORM_LINUX)
#	include <signal.h>
#	define QK_DEBUGBREAK() raise(SIGTRAP)
#elif defined(QK_PLATFORM_APPLE)
#	define QK_DEBUGBREAK() abort()
#else
#	define QK_DEBUGBREAK()
#endif

// Configuration
#if defined(_DEBUG)
#	define QK_DEBUG
#else
#	define QK_RELEASE
#endif

// Standards configuration
#define GLM_FORCE_LEFT_HANDED
#include "../../../vendor/glm/glm.hpp"
#include "../../../vendor/glm/gtc/matrix_transform.hpp"
#include "../../../vendor/glm/gtc/quaternion.hpp"
#include "../../../vendor/glm/gtx/quaternion.hpp"

#include "Logger.h"
#include "../Tools/LogUtils.h"
#include "../Debug/Monitoring.h"

#define QK_FATAL(...) { QK_CORE_ERROR(__VA_ARGS__); QK_DEBUGBREAK(); }

#define QK_ASSERT(x, ...) { if(!(x)) { QK_CORE_ERROR(__VA_ARGS__); QK_DEBUGBREAK(); } }

#define ATTACH_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

#define BIT(x) (1 << x)

#define QK_FLIP_ON_LOAD 1

namespace Quark {

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
