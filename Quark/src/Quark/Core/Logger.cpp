#include "Logger.h"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

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

		spdlog::register_logger(m_CoreLogger);
		spdlog::register_logger(m_ClientLogger);
		spdlog::register_logger(m_ProfilerLogger);

#if defined(QK_DEBUG)
		m_CoreLogger->set_level(spdlog::level::trace);
		m_ClientLogger->set_level(spdlog::level::trace);
		m_ProfilerLogger->set_level(spdlog::level::trace);
#elif defined(QK_RELEASE)
		m_ProfilerLogger->set_level(spdlog::level::trace);
#endif
	}

	Logger& Logger::GetInstance()
	{
		static Logger s_Instance;
		return s_Instance;
	}
}
