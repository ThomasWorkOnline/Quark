#pragma once

#include "CoreMinimal.h"

// define QK_FORCE_ENABLE_CONSOLE_LOGS 1 to force logs on dist builds (see Core/Tweaks.h)
#if defined(QK_DEBUG) || defined(QK_RELEASE) || QK_FORCE_ENABLE_CONSOLE_LOGS
	#define QK_LOGGING_ENABLED 1
#else
	#define QK_LOGGING_ENABLED 0
#endif

// define QK_FORCE_ENABLE_PROFILING 1 to force profiling (see Core/Tweaks.h)
#if defined(QK_DEBUG) || defined(QK_RELEASE) || QK_FORCE_ENABLE_PROFILING
	#define QK_PROFILING_ENABLED 1
#else
	#define QK_PROFILING_ENABLED 0
#endif

#define SPDLOG_COMPILED_LIB
#include <spdlog/fmt/fmt.h>

// GLM standards configuration
#include "Quark/Math/Types.h"

#include "Timer.h"
#include "Timestep.h"
#include "Platform.h"

#include "Quark/Debug/Logger.h"
#include "Quark/Debug/LogUtils.h"
#include "Quark/Debug/Profiling.h"

#include <stdexcept>
#include <utility>

#define ThrowRuntimeError(...) throw std::runtime_error(fmt::format(__VA_ARGS__))
#define Verify(x, ...) do { if (!(x)) { ThrowRuntimeError(__VA_ARGS__); } } while (false)

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

#define ATTACH_EVENT_FN(fn) [&](auto&&... args) -> decltype(auto) { return fn(std::forward<decltype(args)>(args)...); }
#define BIT(x) (1 << x)

#define sizeof_array(x) (sizeof(x) / sizeof(x[0]))

namespace Quark {

	namespace RuntimeCore {

		void Init();
		void Shutdown();
	}
}
