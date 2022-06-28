#include "qkpch.h"
#include "Profiling.h"

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

#if defined(QK_ENABLE_PROFILE_DUMP) && defined(QK_ENABLE_PROFILING)
			InstrumentorProfile result;
			result.ScopeName = m_Scope;
			result.Start = std::chrono::time_point_cast<std::chrono::microseconds>(m_Start).time_since_epoch();
			result.Duration = elapsed;
			result.ThreadID = std::this_thread::get_id();

			Instrumentor::Get().WriteProfile(result);
#endif

#if defined(QK_ENABLE_PROFILE_LOG) && defined(QK_ENABLE_PROFILING)
			QK_CORE_INFO("'{0}'\t took: {1}us ({2}ms)", m_Scope, elapsed.count(), elapsed.count() / 1000.f);
#endif
		}
	}
}
