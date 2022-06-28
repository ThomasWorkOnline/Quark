#include "qkpch.h"
#include "Timer.h"

namespace Quark {

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

	std::chrono::milliseconds Timer::Milliseconds() const
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(m_End - m_Start);
	}
}
