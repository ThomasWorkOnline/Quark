#include "Monitoring.h"

namespace Quark {

	ScopeTimer::ScopeTimer(const char* scope)
		: m_Scope(scope)
	{
		m_Start = std::chrono::steady_clock::now();
	}

	static std::mutex s_WriterMutex;
	ScopeTimer::~ScopeTimer()
	{
		m_End = std::chrono::steady_clock::now();
		m_Elapsed = m_End - m_Start;

		std::lock_guard<std::mutex> lock(s_WriterMutex);
		std::cout << "[TIMER]: " << m_Scope << " took:\t" << m_Elapsed.count() * 1000.0f << "ms\n";
	}
}
