#pragma once

#include "../Core/Core.h"

#include <chrono>

#define QK_FORCE_TIME_SCOPE_RELEASE

#if defined(QK_DEBUG) || defined(QK_FORCE_TIME_SCOPE_RELEASE)
#	define QK_TIME_SCOPE_DEBUG(scope) Quark::ScopeTimer scopeTimer(#scope)
#else
#	define QK_TIME_SCOPE_DEBUG(scope)
#endif

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
		std::chrono::duration<double> m_Elapsed{};
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
