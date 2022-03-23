#pragma once

#include "Quark/Core/Core.h"

#ifdef QK_DEBUG
#	define QK_ENABLE_PROFILING
#endif

#ifdef QK_ENABLE_PROFILING
#	define QK_SCOPE_TIMER(scope) Quark::ScopeTimer scopeTimer(#scope)
#else
#	define QK_SCOPE_TIMER(scope)
#endif

#include <chrono>

namespace Quark {

	class Timer
	{
	public:
		void Start();
		void Stop();

		double Milliseconds() const { return m_Elapsed.count() * 1000.0; }

	protected:
		std::chrono::steady_clock::time_point m_Start;
		std::chrono::steady_clock::time_point m_End;
		std::chrono::duration<double> m_Elapsed{ 0.0 };
	};

	class ScopeTimer : public Timer
	{
	public:
		ScopeTimer(const char* scope);
		~ScopeTimer();

	private:
		const char* m_Scope;
	};
}