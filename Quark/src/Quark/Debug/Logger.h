#pragma once

#include "Quark/Core/Core.h"

#if defined(QK_DEBUG) || defined(QK_RELEASE)
#	ifndef QK_ENABLE_CONSOLE_LOG
#		define QK_ENABLE_CONSOLE_LOG // <--- define QK_ENABLE_CONSOLE_LOG to force logs on dist builds (see Core/Tweaks.h)
#	endif
#endif

#pragma warning(push, 0)
#define SPDLOG_COMPILED_LIB
#include <spdlog/spdlog.h>
#pragma warning(pop)

#include <fstream>
#include <mutex>

namespace Quark {

	struct Logger
	{
		Logger();

		static const Ref<spdlog::logger>& GetCoreLogger() { return GetInstance()->m_CoreLogger; }
		static const Ref<spdlog::logger>& GetClientLogger() { return GetInstance()->m_ClientLogger; }

		static std::mutex&                GetOutputLock() { return GetInstance()->m_OutputMutex; }
		static std::ofstream&             GetOutputStream() { return GetInstance()->m_Output; }

		static void DumpToFile(const char* string);
		static Logger* GetInstance();

	private:
		Ref<spdlog::logger> m_CoreLogger;
		Ref<spdlog::logger> m_ClientLogger;

		std::mutex m_OutputMutex;
		std::ofstream m_Output;
	};
}

#ifdef QK_ENABLE_CONSOLE_LOG
#	define QK_CORE_CONSOLE_TRACE(...)    ::Quark::Logger::GetCoreLogger()->trace   (__VA_ARGS__)
#	define QK_CORE_CONSOLE_INFO(...)     ::Quark::Logger::GetCoreLogger()->info    (__VA_ARGS__)
#	define QK_CORE_CONSOLE_WARN(...)     ::Quark::Logger::GetCoreLogger()->warn    (__VA_ARGS__)
#	define QK_CORE_CONSOLE_ERROR(...)    ::Quark::Logger::GetCoreLogger()->error   (__VA_ARGS__)
#	define QK_CORE_CONSOLE_CRITICAL(...) ::Quark::Logger::GetCoreLogger()->critical(__VA_ARGS__)

#	define QK_CLIENT_CONSOLE_TRACE(...)    ::Quark::Logger::GetClientLogger()->trace   (__VA_ARGS__)
#	define QK_CLIENT_CONSOLE_INFO(...)     ::Quark::Logger::GetClientLogger()->info    (__VA_ARGS__)
#	define QK_CLIENT_CONSOLE_WARN(...)     ::Quark::Logger::GetClientLogger()->warn    (__VA_ARGS__)
#	define QK_CLIENT_CONSOLE_ERROR(...)    ::Quark::Logger::GetClientLogger()->error   (__VA_ARGS__)
#	define QK_CLIENT_CONSOLE_CRITICAL(...) ::Quark::Logger::GetClientLogger()->critical(__VA_ARGS__)
#else
#	define QK_CORE_CONSOLE_TRACE(...)
#	define QK_CORE_CONSOLE_INFO(...)
#	define QK_CORE_CONSOLE_WARN(...)
#	define QK_CORE_CONSOLE_ERROR(...)
#	define QK_CORE_CONSOLE_CRITICAL(...)

#	define QK_CLIENT_CONSOLE_TRACE(...)
#	define QK_CLIENT_CONSOLE_INFO(...)
#	define QK_CLIENT_CONSOLE_WARN(...)
#	define QK_CLIENT_CONSOLE_ERROR(...)
#	define QK_CLIENT_CONSOLE_CRITICAL(...)
#endif

#ifdef QK_ENABLE_DUMP_LOG
#	define QK_DUMP_TRACE(...)    ::Quark::Logger::DumpToFile(("[Trace]:    " + fmt::format(__VA_ARGS__)).c_str())
#	define QK_DUMP_INFO(...)     ::Quark::Logger::DumpToFile(("[Info]:     " + fmt::format(__VA_ARGS__)).c_str())
#	define QK_DUMP_WARN(...)     ::Quark::Logger::DumpToFile(("[Warn]:     " + fmt::format(__VA_ARGS__)).c_str())
#	define QK_DUMP_ERROR(...)    ::Quark::Logger::DumpToFile(("[ERROR]:    " + fmt::format(__VA_ARGS__)).c_str())
#	define QK_DUMP_CRITICAL(...) ::Quark::Logger::DumpToFile(("[CRITICAL]: " + fmt::format(__VA_ARGS__)).c_str())
#else
#	define QK_DUMP_TRACE(...)
#	define QK_DUMP_INFO(...)
#	define QK_DUMP_WARN(...)
#	define QK_DUMP_ERROR(...)
#	define QK_DUMP_CRITICAL(...)
#endif
