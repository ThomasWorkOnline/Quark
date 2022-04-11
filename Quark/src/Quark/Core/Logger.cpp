#include "Logger.h"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

// from consoleapi2.h
#define FOREGROUND_BLUE      0x0001 // text color contains blue.
#define FOREGROUND_GREEN     0x0002 // text color contains green.
#define FOREGROUND_RED       0x0004 // text color contains red.
#define FOREGROUND_INTENSITY 0x0008 // text color is intensified.
#define BACKGROUND_BLUE      0x0010 // background color contains blue.
#define BACKGROUND_GREEN     0x0020 // background color contains green.
#define BACKGROUND_RED       0x0040 // background color contains red.
#define BACKGROUND_INTENSITY 0x0080 // background color is intensified.

namespace Quark {

	Logger::Logger()
	{
		auto logSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
		logSink->set_color(spdlog::level::trace, FOREGROUND_INTENSITY);
		logSink->set_color(spdlog::level::info, FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
		logSink->set_pattern("%^[%n]:%$ %v");

		std::vector<spdlog::sink_ptr> sinks;
		sinks.push_back(logSink);

		m_CoreLogger = std::make_shared<spdlog::logger>("Core", sinks.begin(), sinks.end());
		m_ClientLogger = std::make_shared<spdlog::logger>("Client", sinks.begin(), sinks.end());
		m_ProfilerLogger = std::make_shared<spdlog::logger>("Profiler", sinks.begin(), sinks.end());

#if defined(QK_DEBUG)
		m_CoreLogger->set_level(spdlog::level::trace);
		m_ClientLogger->set_level(spdlog::level::trace);
		m_ProfilerLogger->set_level(spdlog::level::trace);
#elif defined(QK_RELEASE)
		m_ProfilerLogger->set_level(spdlog::level::trace);
#endif

		spdlog::register_logger(m_CoreLogger);
		spdlog::register_logger(m_ClientLogger);
		spdlog::register_logger(m_ProfilerLogger);
	}

	Logger& Logger::GetInstance()
	{
		static Logger s_Instance;
		return s_Instance;
	}
}
