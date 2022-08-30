#pragma once

#include "Tweaks.h"
#include "Config.h"
#include "Platform.h"
#include "Assertions.h"

#include <utility>

#if __cplusplus >= 202002L
	// C++20 and forward
#	define QK_CONSTEXPR20 constexpr
#else
#	define QK_CONSTEXPR20 inline
#endif

#define QK_NODISCARD [[nodiscard]]

#ifndef QK_IGNORE_DEPRECATED_SYMBOLS
#	define QK_DEPRECATED [[deprecated]]
#else
#	define QK_DEPRECATED
#endif

#define ATTACH_EVENT_FN(fn) [&](auto&&... args) -> decltype(auto) { return fn(std::forward<decltype(args)>(args)...); }
#define BIT(x) (1 << x)

#include "Timer.h"
#include "Timestep.h"
#include "Memory.h"

// GLM standards configuration
#include "Quark/Math/Types.h"

#include "Quark/Debug/Logger.h"
#include "Quark/Debug/LogUtils.h"
#include "Quark/Debug/Profiling.h"

////////////////////////////////////////////////////////////////
// Logging Macros
// --------------

#define QK_CORE_TRACE(...)    do { QK_CORE_CONSOLE_TRACE(__VA_ARGS__);    QK_DUMP_TRACE(__VA_ARGS__);   } while (false)
#define QK_CORE_INFO(...)     do { QK_CORE_CONSOLE_INFO(__VA_ARGS__);     QK_DUMP_INFO(__VA_ARGS__);    } while (false)
#define QK_CORE_WARN(...)     do { QK_CORE_CONSOLE_WARN(__VA_ARGS__);     QK_DUMP_WARN(__VA_ARGS__);    } while (false)
#define QK_CORE_ERROR(...)    do { QK_CORE_CONSOLE_ERROR(__VA_ARGS__);    QK_DUMP_ERROR(__VA_ARGS__);   } while (false)
#define QK_CORE_CRITICAL(...) do { QK_CORE_CONSOLE_CRITICAL(__VA_ARGS__); QK_DUMP_CRITICAL(__VA_ARGS__);} while (false)

#define QK_TRACE(...)         do { QK_CLIENT_CONSOLE_TRACE(__VA_ARGS__);    QK_DUMP_TRACE(__VA_ARGS__);    } while (false)
#define QK_INFO(...)          do { QK_CLIENT_CONSOLE_INFO(__VA_ARGS__);     QK_DUMP_INFO(__VA_ARGS__);     } while (false)
#define QK_WARN(...)          do { QK_CLIENT_CONSOLE_WARN(__VA_ARGS__);     QK_DUMP_WARN(__VA_ARGS__);     } while (false)
#define QK_ERROR(...)         do { QK_CLIENT_CONSOLE_ERROR(__VA_ARGS__);    QK_DUMP_ERROR(__VA_ARGS__);    } while (false)
#define QK_CRITICAL(...)      do { QK_CLIENT_CONSOLE_CRITICAL(__VA_ARGS__); QK_DUMP_CRITICAL(__VA_ARGS__); } while (false)

namespace Quark {

	namespace RuntimeCore {

		void Init();
		void Shutdown();
	}
}
