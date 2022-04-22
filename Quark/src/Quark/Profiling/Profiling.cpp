#include "qkpch.h"
#include "Profiling.h"

#include <mutex>

namespace Quark {

	void Timer::Start()
	{
		m_Start = std::chrono::steady_clock::now();
	}

	void Timer::Stop()
	{
		auto end = std::chrono::steady_clock::now();
		m_Elapsed = end - m_Start;
	}

	ScopeTimer::ScopeTimer(const char* scope)
		: m_Scope(scope)
	{
		Start();
	}

	ScopeTimer::~ScopeTimer()
	{
		Stop();
		Logger::GetProfilerLogger()->debug("'{0}'\t took: {1}ms", m_Scope, (float)Milliseconds());
	}
}
