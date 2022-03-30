#include "Logger.h"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace Quark {

	Ref<spdlog::logger> Logger::s_CoreLogger;
	Ref<spdlog::logger> Logger::s_ClientLogger;
	Ref<spdlog::logger> Logger::s_ProfilerLogger;

	void Logger::Init()
	{
		auto logSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
		logSink->set_color(spdlog::level::trace, FOREGROUND_INTENSITY);
		logSink->set_color(spdlog::level::info, FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
		logSink->set_pattern("%^[%n]:%$ %v");

		std::vector<spdlog::sink_ptr> sinks;
		sinks.push_back(logSink);

		s_CoreLogger     = std::make_shared<spdlog::logger>("Core", sinks.begin(), sinks.end());
		s_ClientLogger   = std::make_shared<spdlog::logger>("Client", sinks.begin(), sinks.end());
		s_ProfilerLogger = std::make_shared<spdlog::logger>("Profiler", sinks.begin(), sinks.end());

		spdlog::register_logger(s_CoreLogger);
		spdlog::register_logger(s_ClientLogger);
		spdlog::register_logger(s_ProfilerLogger);

#if defined(QK_DEBUG)
		s_CoreLogger->set_level(spdlog::level::trace);
		s_ClientLogger->set_level(spdlog::level::trace);
		s_ProfilerLogger->set_level(spdlog::level::trace);
#elif defined(QK_RELEASE)
		s_ProfilerLogger->set_level(spdlog::level::trace);
#endif
	}
}
