#pragma once

#include <chrono>

namespace Quark {

	class Timer
	{
	public:
		void Start();
		void Stop();

		std::chrono::microseconds Microseconds() const;
		std::chrono::milliseconds Milliseconds() const;

	protected:
		std::chrono::steady_clock::time_point m_Start;
		std::chrono::steady_clock::time_point m_End;
	};
}
