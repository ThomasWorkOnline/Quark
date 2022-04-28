#pragma once

#include "Quark/Core/Core.h"
#include "Quark/Core/FuncSig.h"

#include <chrono>
#include <fstream>
#include <mutex>

#if defined(QK_DEBUG) || defined(QK_RELEASE)
#	define QK_ENABLE_PROFILING // <--- define QK_ENABLE_PROFILING before including quark to force profiling
#endif

//                                   vvv--- set this to true to enable verbose signatures
#define QK_VERBOSE_FUNCTION_SIG     false
#define QK_ENABLE_PROFILE_LOG       false

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

		class Timer
		{
		public:
			Timer(const char* scope);
			~Timer();

			void Stop();

		private:
			std::chrono::steady_clock::time_point m_Start;
			const char* m_Scope;
		};
	}
}

#define CONCAT_IMPL(a, b) a ## b
#define CONCAT(a, b) CONCAT_IMPL(a, b)

#ifdef QK_ENABLE_PROFILING
#	define QK_PROFILE_SCOPE(scope) ::Quark::Profile::Timer CONCAT(profiler, __LINE__)(#scope)
#	define QK_PROFILE_FUNCTION()   ::Quark::Profile::Timer CONCAT(profiler, __LINE__)(QK_FUNCTION_SIG)

#	define QK_BEGIN_PROFILE_SESSION(sessionName) ::Quark::Profile::Instrumentor::Get().BeginSession(sessionName);
#	define QK_END_PROFILE_SESSION()              ::Quark::Profile::Instrumentor::Get().EndSession();
#else
#	define QK_PROFILE_SCOPE(scope)
#	define QK_PROFILE_FUNCTION()

#	define QK_BEGIN_PROFILE_SESSION(sessionName)
#	define QK_END_PROFILE_SESSION()
#endif
