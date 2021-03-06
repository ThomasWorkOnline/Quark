#include "Monitoring.h"

namespace Entropy {

	ScopeTimer::ScopeTimer(const char* scope)
		: m_Scope(scope)
	{
		m_Start = std::chrono::steady_clock::now();
	}

	ScopeTimer::~ScopeTimer()
	{
		m_End = std::chrono::steady_clock::now();
		m_Elapsed = m_End - m_Start;
		std::cout << m_Scope << " took:\t" << m_Elapsed.count() * 1000.0f << "ms         \r";
	}
}
