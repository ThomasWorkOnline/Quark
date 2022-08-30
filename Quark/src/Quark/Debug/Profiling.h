#pragma once

#include "Quark/Core/Core.h"
#include "Quark/Core/Timer.h"

#include "FuncSig.h"
#include "Logger.h"

#include <chrono>
#include <fstream>
#include <mutex>
#include <thread>

#if defined(QK_DEBUG) || defined(QK_RELEASE)
#	ifndef QK_ENABLE_PROFILING
#		define QK_ENABLE_PROFILING // <--- define QK_ENABLE_PROFILING to force profiling (see Core/Tweaks.h)
#	endif
#endif

#ifdef QK_ENABLE_PROFILING
#	define CONCAT_IMPL(a, b) a ## b
#	define CONCAT(a, b) CONCAT_IMPL(a, b)

#	define QK_PROFILE_SCOPE(scope) ::Quark::Profile::ScopeTimer CONCAT(profiler, __LINE__)(#scope)
#	define QK_PROFILE_FUNCTION()   ::Quark::Profile::ScopeTimer CONCAT(profiler, __LINE__)(QK_FUNCTION_SIG)

#	define QK_BEGIN_PROFILE_SESSION(sessionName) ::Quark::Profile::Instrumentor::Get().BeginSession(sessionName);
#	define QK_END_PROFILE_SESSION()              ::Quark::Profile::Instrumentor::Get().EndSession();
#else
#	define QK_PROFILE_SCOPE(scope)
#	define QK_PROFILE_FUNCTION()

#	define QK_BEGIN_PROFILE_SESSION(sessionName)
#	define QK_END_PROFILE_SESSION()
#endif

namespace Quark {

	namespace Profile {

		struct InstrumentorProfile
		{
			const char* ScopeName;
			std::chrono::microseconds Start;
			std::chrono::microseconds Duration;
			std::thread::id ThreadID;
		};

		class Instrumentor
		{
		public:
			void BeginSession(const std::string& sessionName);
			void EndSession();
			void WriteProfile(const InstrumentorProfile& result);

			static Instrumentor& Get();

		private:
			void WriteHeader();
			void WriteFooter();

			std::mutex m_OutputMutex;
			std::ofstream m_Output;
		};

		class ScopeTimer : public Timer
		{
		public:
			ScopeTimer(const char* scope);
			~ScopeTimer();

			void Stop();

		private:
			const char* m_ScopeName;
		};
	}
}
