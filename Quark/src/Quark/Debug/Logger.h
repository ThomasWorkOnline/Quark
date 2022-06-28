#pragma once

#include "Quark/Core/Core.h"

#if defined(QK_DEBUG) || defined(QK_RELEASE)
#	ifndef QK_ENABLE_CONSOLE_LOG
#		define QK_ENABLE_CONSOLE_LOG // <--- define QK_ENABLE_CONSOLE_LOG to force logs on dist builds (see Core/Tweaks.h)
#	endif
#endif

#ifdef QK_ENABLE_CONSOLE_LOG
#	pragma warning(push, 0)
#	define SPDLOG_COMPILED_LIB
#	include <spdlog/spdlog.h>
#	pragma warning(pop)

namespace Quark {

	struct Logger
	{
		Logger();

		static Ref<spdlog::logger> GetCoreLogger() { return GetInstance().m_CoreLogger; }
		static Ref<spdlog::logger> GetClientLogger() { return GetInstance().m_ClientLogger; }

		static Logger& GetInstance();

	private:
		Ref<spdlog::logger> m_CoreLogger;
		Ref<spdlog::logger> m_ClientLogger;
	};
}

#	define QK_CORE_TRACE(...)    ::Quark::Logger::GetCoreLogger()->trace   (__VA_ARGS__)
#	define QK_CORE_INFO(...)     ::Quark::Logger::GetCoreLogger()->info    (__VA_ARGS__)
#	define QK_CORE_WARN(...)     ::Quark::Logger::GetCoreLogger()->warn    (__VA_ARGS__)
#	define QK_CORE_ERROR(...)    ::Quark::Logger::GetCoreLogger()->error   (__VA_ARGS__)
#	define QK_CORE_CRITICAL(...) ::Quark::Logger::GetCoreLogger()->critical(__VA_ARGS__)

#	define QK_TRACE(...)         ::Quark::Logger::GetClientLogger()->trace   (__VA_ARGS__)
#	define QK_INFO(...)          ::Quark::Logger::GetClientLogger()->info    (__VA_ARGS__)
#	define QK_WARN(...)          ::Quark::Logger::GetClientLogger()->warn    (__VA_ARGS__)
#	define QK_ERROR(...)         ::Quark::Logger::GetClientLogger()->error   (__VA_ARGS__)
#	define QK_CRITICAL(...)      ::Quark::Logger::GetClientLogger()->critical(__VA_ARGS__)

#else

#	define QK_CORE_TRACE(...)
#	define QK_CORE_INFO(...)
#	define QK_CORE_WARN(...)
#	define QK_CORE_ERROR(...)
#	define QK_CORE_CRITICAL(...)

#	define QK_TRACE(...)
#	define QK_INFO(...)
#	define QK_WARN(...)
#	define QK_ERROR(...)
#	define QK_CRITICAL(...)

#endif /*QK_ENABLE_CONSOLE_LOG*/
