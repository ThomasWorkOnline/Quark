#pragma once

// Platform detection
#include "Platform.h"

#include <functional>
#include <memory>

#define ATTACH_EVENT_FN(fn) [&](auto&&... args) -> decltype(auto) { return fn(std::forward<decltype(args)>(args)...); }
#define BIT(x) (1 << x)

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
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

	template<typename T, typename Deleter, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::shared_ptr<T>(new T(std::forward<Args>(args)...), Deleter());
	}
}

// GLM standards configuration
#include "Quark/Math/Types.h"

#include "Assertions.h"
#include "FuncSig.h"
#include "Logger.h"
#include "Timestep.h"

#include "Quark/Profiling/MemoryMetrics.h"
#include "Quark/Profiling/Profiling.h"
