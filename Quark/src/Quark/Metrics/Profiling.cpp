#include "qkpch.h"
#include "Profiling.h"

#include <filesystem>
#include <iomanip>
#include <sstream>

#if defined(QK_DEBUG)
#	define CONFIG_NAME "debug"
#elif defined(QK_RELEASE)
#	define CONFIG_NAME "release"
#elif defined(QK_DIST)
#	define CONFIG_NAME "dist"
#else
#	warning "Undefined build configuration assuming debug"
#endif

namespace Quark {

	namespace Profile {

		struct InstrumentorSettings
		{
			static inline const char* OutputDir = "instrumentor/" CONFIG_NAME "/";
		};

		void Instrumentor::BeginSession(const std::string& sessionName)
		{
			std::filesystem::create_directories(InstrumentorSettings::OutputDir);
			m_Output.open(InstrumentorSettings::OutputDir + sessionName);
			WriteHeader();
		}

		void Instrumentor::EndSession()
		{
			WriteFooter();
			m_Output.close();
		}

		void Instrumentor::WriteProfile(const InstrumentorProfile& result)
		{
			std::stringstream json;

			json << std::setprecision(3) << std::fixed;
			json << ",{";
			json << "\"cat\":\"function\",";
			json << "\"dur\":" << result.Duration.count() << ',';
			json << "\"name\":\"" << result.ScopeName << "\",";
			json << "\"ph\":\"X\",";
			json << "\"pid\":0,";
			json << "\"tid\":" << result.ThreadID << ',';
			json << "\"ts\":" << result.Start.count();
			json << "}";

			std::lock_guard lock(m_OutputMutex);
			m_Output << json.str();
			m_Output.flush();
		}

		Instrumentor& Instrumentor::Get()
		{
			static Instrumentor instance;
			return instance;
		}

		void Instrumentor::WriteHeader()
		{
			std::lock_guard lock(m_OutputMutex);
			m_Output << "{\"otherData\": {},\"traceEvents\":[{}";
			m_Output.flush();
		}

		void Instrumentor::WriteFooter()
		{
			std::lock_guard lock(m_OutputMutex);
			m_Output << "]}";
			m_Output.flush();
		}

		void Timer::Start()
		{
			m_Start = std::chrono::steady_clock::now();
		}

		void Timer::Stop()
		{
			m_End = std::chrono::steady_clock::now();
		}

		std::chrono::microseconds Timer::Microseconds() const
		{
			return std::chrono::duration_cast<std::chrono::microseconds>(m_End - m_Start);
		}

		ScopeTimer::ScopeTimer(const char* scope)
			: m_Scope(scope)
		{
			Start();
		}

		ScopeTimer::~ScopeTimer()
		{
			Stop();
		}

		void ScopeTimer::Stop()
		{
			Timer::Stop();

			auto elapsed = Microseconds();

			InstrumentorProfile result;
			result.ScopeName = m_Scope;
			result.Start = std::chrono::time_point_cast<std::chrono::microseconds>(m_Start).time_since_epoch();
			result.Duration = elapsed;
			result.ThreadID = std::this_thread::get_id();

			Instrumentor::Get().WriteProfile(result);

			if (elapsed.count() > 10000) // > 10ms
			{
				Logger::GetProfilerLogger()->warn("'{0}'\t took: {1}us ({2}ms)", m_Scope, elapsed.count(), elapsed.count() / 1000.f);
			}
			else
			{
#ifdef QK_ENABLE_PROFILE_LOG
				Logger::GetProfilerLogger()->debug("'{0}'\t took: {1}us ({2}ms)", m_Scope, elapsed.count(), elapsed.count() / 1000.f);
#endif
			}
		}
	}
}
