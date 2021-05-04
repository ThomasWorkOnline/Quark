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

	class ScopeTimer
	{
	public:
		ScopeTimer(const char* scope);
		~ScopeTimer();

	private:
		std::chrono::steady_clock::time_point m_Start;
		std::chrono::steady_clock::time_point m_End;	
		std::chrono::duration<double> m_Elapsed = {};
		const char* m_Scope;
	};
}
