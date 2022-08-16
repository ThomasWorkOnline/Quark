#pragma once

// Platform detection
#include "Config.h"
#include "Platform.h"
#include "Tweaks.h"
#include "Memory.h"

#include <functional>

#define ATTACH_EVENT_FN(fn) [&](auto&&... args) -> decltype(auto) { return fn(std::forward<decltype(args)>(args)...); }
#define BIT(x) (1 << x)

#if __cplusplus >= 202002L
	// C++20 and later
#	define QK_CONSTEXPR20 constexpr
#else
#	define QK_CONSTEXPR20 inline
#endif

#ifndef QK_IGNORE_DEPRECATED_SYMBOLS
#	define QK_DEPRECATED [[deprecated]]
#else
#	define QK_DEPRECATED
#endif

// GLM standards configuration
#include "Quark/Math/Types.h"

#include "Assertions.h"
#include "Timer.h"
#include "Timestep.h"

#include "Quark/Debug/Logger.h"
#include "Quark/Debug/LogUtils.h"
#include "Quark/Debug/Profiling.h"
#include "Quark/Debug/MemoryContext.h"

namespace Quark {

	namespace RuntimeCore {

		void Init();
		void Shutdown();
	}
}
