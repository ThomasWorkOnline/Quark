#pragma once

#include "Core.h"

#include <spdlog/spdlog.h>

namespace Quark {

	struct Logger
	{
		static void Init();
		static Ref<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		static Ref<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
		static Ref<spdlog::logger>& GetProfilerLogger() { return s_ProfilerLogger; }

	private:
		static Ref<spdlog::logger> s_CoreLogger;
		static Ref<spdlog::logger> s_ClientLogger;
		static Ref<spdlog::logger> s_ProfilerLogger;
	};
}

#define QK_CORE_TRACE(...)    Logger::GetCoreLogger()->trace    (__VA_ARGS__);
#define QK_CORE_INFO(...)     Logger::GetCoreLogger()->info     (__VA_ARGS__);
#define QK_CORE_WARN(...)     Logger::GetCoreLogger()->warn     (__VA_ARGS__);
#define QK_CORE_ERROR(...)    Logger::GetCoreLogger()->error    (__VA_ARGS__);
#define QK_CORE_CRITICAL(...) Logger::GetCoreLogger()->critical (__VA_ARGS__);

#define QK_TRACE(...)         Logger::GetClientLogger()->trace     (__VA_ARGS__);
#define QK_INFO(...)          Logger::GetClientLogger()->info      (__VA_ARGS__);
#define QK_WARN(...)          Logger::GetClientLogger()->warn      (__VA_ARGS__);
#define QK_ERROR(...)         Logger::GetClientLogger()->error     (__VA_ARGS__);
#define QK_CRITICAL(...)      Logger::GetClientLogger()->critical  (__VA_ARGS__);

#include "LogUtils.h"
