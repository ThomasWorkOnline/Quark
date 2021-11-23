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
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Logger.h"
#include "../Tools/LogUtils.h"
#include "../Profiling/Monitoring.h"

#define QK_FATAL(...) do { QK_CORE_ERROR(__VA_ARGS__); QK_DEBUGBREAK(); } while (false)

#ifdef QK_DEBUG
#	define QK_ASSERT(x, ...) do { if(!(x)) { QK_CORE_ERROR(__VA_ARGS__); QK_DEBUGBREAK(); } } while (false)
#else
#	define QK_ASSERT(x, ...)
#endif

#define ATTACH_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

#define BIT(x) (1 << x)

#define QK_FLIP_ON_LOAD 1

namespace Quark {

	template<typename T, typename Deleter = std::default_delete<T>>
	using Scope = std::unique_ptr<T, Deleter>;
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
		return std::make_shared(std::forward<Args>(args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename Deleter = std::default_delete<T>, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::shared_ptr<T>(new T(std::forward<Args>(args)...), Deleter());
	}
}
