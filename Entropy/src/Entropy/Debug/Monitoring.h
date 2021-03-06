#pragma once

#include "../Core/Core.h"

#include <chrono>

#if defined(NT_DEBUG)
#	define NT_TIME_SCOPE_DEBUG(scope) Entropy::ScopeTimer scopeTimer(#scope)
#else
#	define NT_TIME_SCOPE_DEBUG(scope)
#endif

namespace Entropy {

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
