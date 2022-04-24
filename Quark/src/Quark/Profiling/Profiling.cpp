#include "qkpch.h"
#include "Profiling.h"

#include <filesystem>
#include <iomanip>
#include <sstream>

namespace Quark {

	namespace Profiling {

		struct InstrumentorSettings
		{
			static constexpr char* OutputDir = "instrumentor/" QK_CONFIG "/";
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
			json << "\"dur\":" << (result.Duration.count()) << ',';
			json << "\"name\":\"" << result.Scope << "\",";
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

		Timer::Timer(const char* scope)
			: m_Scope(scope)
		{
			m_Start = std::chrono::steady_clock::now();
		}

		Timer::~Timer()
		{
			Stop();
		}

		void Timer::Stop()
		{
			auto end = std::chrono::steady_clock::now();
			auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - m_Start);

			InstrumentorProfile result;
			result.Scope = m_Scope;
			result.Start = std::chrono::time_point_cast<std::chrono::microseconds>(m_Start).time_since_epoch();
			result.Duration = elapsed;
			result.ThreadID = std::this_thread::get_id();

			Instrumentor::Get().WriteProfile(result);

#if QK_ENABLE_PROFILE_LOG
			Logger::GetProfilerLogger()->debug("'{0}'\t took: {1}us ({2}ms)", m_Scope, elapsed.count(), elapsed.count() / 1000.f);
#endif
		}
	}
}
