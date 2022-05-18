#pragma once

#include "Core.h"

#define SPDLOG_COMPILED_LIB
#include <spdlog/spdlog.h>

namespace Quark {

	struct Logger
	{
		Logger();

		static Ref<spdlog::logger> GetCoreLogger() { return GetInstance().m_CoreLogger; }
		static Ref<spdlog::logger> GetClientLogger() { return GetInstance().m_ClientLogger; }
		static Ref<spdlog::logger> GetProfilerLogger() { return GetInstance().m_ProfilerLogger; }

		static Logger& GetInstance();

	private:
		Ref<spdlog::logger> m_CoreLogger;
		Ref<spdlog::logger> m_ClientLogger;
		Ref<spdlog::logger> m_ProfilerLogger;
	};
}

#define QK_CORE_TRACE(...)    ::Quark::Logger::GetCoreLogger()->trace   (__VA_ARGS__)
#define QK_CORE_INFO(...)     ::Quark::Logger::GetCoreLogger()->info    (__VA_ARGS__)
#define QK_CORE_WARN(...)     ::Quark::Logger::GetCoreLogger()->warn    (__VA_ARGS__)
#define QK_CORE_ERROR(...)    ::Quark::Logger::GetCoreLogger()->error   (__VA_ARGS__)
#define QK_CORE_CRITICAL(...) ::Quark::Logger::GetCoreLogger()->critical(__VA_ARGS__)

#define QK_TRACE(...)         ::Quark::Logger::GetClientLogger()->trace   (__VA_ARGS__)
#define QK_INFO(...)          ::Quark::Logger::GetClientLogger()->info    (__VA_ARGS__)
#define QK_WARN(...)          ::Quark::Logger::GetClientLogger()->warn    (__VA_ARGS__)
#define QK_ERROR(...)         ::Quark::Logger::GetClientLogger()->error   (__VA_ARGS__)
#define QK_CRITICAL(...)      ::Quark::Logger::GetClientLogger()->critical(__VA_ARGS__)

#include "LogUtils.h"
