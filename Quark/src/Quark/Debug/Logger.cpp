#include "qkpch.h"
#include "Logger.h"

#ifdef QK_ENABLE_CONSOLE_LOG

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#ifdef QK_PLATFORM_WINDOWS
#	include <Windows.h>
#endif

namespace Quark {

	Logger::Logger()
	{
		std::vector<spdlog::sink_ptr> sinks;
		auto logSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

#if defined(QK_PLATFORM_WINDOWS)
		logSink->set_color(spdlog::level::trace, FOREGROUND_INTENSITY);
		logSink->set_color(spdlog::level::info, FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
#endif

		logSink->set_pattern("%^[%n]:%$ %v");
		sinks.push_back(logSink);

		m_CoreLogger = std::make_shared<spdlog::logger>("Core", sinks.begin(), sinks.end());
		m_ClientLogger = std::make_shared<spdlog::logger>("Client", sinks.begin(), sinks.end());
		
#if defined(QK_DEBUG)
		m_CoreLogger->set_level(spdlog::level::trace);
		m_ClientLogger->set_level(spdlog::level::trace);
#else
		m_CoreLogger->set_level(spdlog::level::info);
		m_ClientLogger->set_level(spdlog::level::info);
#endif

		spdlog::register_logger(m_CoreLogger);
		spdlog::register_logger(m_ClientLogger);
	}

	Logger& Logger::GetInstance()
	{
		static Logger instance;
		return instance;
	}
}

#endif /*QK_ENABLE_CONSOLE_LOG*/
