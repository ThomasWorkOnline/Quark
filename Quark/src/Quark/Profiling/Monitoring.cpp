#include "Monitoring.h"

namespace Quark {

	void Timer::Start()
	{
		m_Start = std::chrono::steady_clock::now();
	}

	void Timer::Stop()
	{
		m_End = std::chrono::steady_clock::now();
		m_Elapsed = m_End - m_Start;
	}

	ScopeTimer::ScopeTimer(const char* scope)
		: m_Scope(scope)
	{
		Start();
	}

	static std::mutex s_WriterMutex;
	ScopeTimer::~ScopeTimer()
	{
		Stop();

		std::lock_guard<std::mutex> lock(s_WriterMutex);
		std::cout << "[TIMER]: " << m_Scope << " took:\t" << Milliseconds() << "ms\n";
	}
}
