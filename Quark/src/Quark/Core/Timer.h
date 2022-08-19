#pragma once

#include <chrono>

namespace Quark {

	class Timer
	{
	public:
		void Start();
		void Stop();

		std::chrono::seconds      Seconds() const;
		std::chrono::milliseconds Milliseconds() const;
		std::chrono::microseconds Microseconds() const;

	protected:
		std::chrono::steady_clock::time_point m_Start;
		std::chrono::steady_clock::time_point m_End;
	};
}
