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

#define QK_CORE_TRACE(...)    Quark::Logger::GetCoreLogger()->trace    (__VA_ARGS__);
#define QK_CORE_INFO(...)     Quark::Logger::GetCoreLogger()->info     (__VA_ARGS__);
#define QK_CORE_WARN(...)     Quark::Logger::GetCoreLogger()->warn     (__VA_ARGS__);
#define QK_CORE_ERROR(...)    Quark::Logger::GetCoreLogger()->error    (__VA_ARGS__);
#define QK_CORE_CRITICAL(...) Quark::Logger::GetCoreLogger()->critical (__VA_ARGS__);

#define QK_TRACE(...)         Quark::Logger::GetClientLogger()->trace     (__VA_ARGS__);
#define QK_INFO(...)          Quark::Logger::GetClientLogger()->info      (__VA_ARGS__);
#define QK_WARN(...)          Quark::Logger::GetClientLogger()->warn      (__VA_ARGS__);
#define QK_ERROR(...)         Quark::Logger::GetClientLogger()->error     (__VA_ARGS__);
#define QK_CRITICAL(...)      Quark::Logger::GetClientLogger()->critical  (__VA_ARGS__);

#include "LogUtils.h"
