#include "qkpch.h"
#include "Profiling.h"

#if QK_PROFILING_ENABLED || QK_PROFILE_LOGS_ENABLED
#	define LogProfileResult(scope, start, elapsed) \
									QK_CORE_INFO("'{0}'\t took: {1}us ({2:.2f}ms)", scope, elapsed.count(), elapsed.count() / 1000.f)
#else
#	define LogProfileResult(scope, start, elapsed)
#endif

#if QK_PROFILING_ENABLED || QK_PROFILE_DUMP_ENABLED
#	define DumpProfileResult(scope, start, elapsed) { InstrumentorProfile result; \
									result.ScopeName = scope; \
									result.Start = std::chrono::time_point_cast<std::chrono::microseconds>(start).time_since_epoch(); \
									result.Duration = elapsed; \
									result.ThreadID = std::this_thread::get_id(); \
									Instrumentor::Get().WriteProfile(result); }
#else
#	define DumpProfileResult(scope, start, elapsed)
#endif

namespace Quark {

	namespace Profile {

		struct InstrumentorSettings
		{
			static inline const char* OutputDir = "instrumentor/" QK_CONFIG_NAME "/";
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

		ScopeTimer::ScopeTimer(const char* scope)
			: m_ScopeName(scope)
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

			DumpProfileResult(m_ScopeName, m_Start, elapsed);
			LogProfileResult(m_ScopeName, m_Start, elapsed);
		}
	}
}
