#pragma once

namespace Quark {

	class Timestep
	{
	public:
		Timestep(float time)
			: m_Time(time)
		{
		}

		float Seconds() const { return m_Time; }
		operator float() const { return m_Time; }

	private:
		float m_Time;
	};
}
