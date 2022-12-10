#pragma once

#include "Quark/Math/Types.h"

namespace Quark {

	class Timestep
	{
	public:
		Timestep() = default;
		Timestep(Float time)
			: m_Time(time)
		{
		}

		Float Seconds() const { return m_Time; }
		operator Float() const { return m_Time; }

	private:
		Float m_Time{};
	};
}
