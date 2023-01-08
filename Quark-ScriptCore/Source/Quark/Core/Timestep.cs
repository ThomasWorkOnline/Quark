namespace Quark
{
	public struct Timestep
	{
		public float Seconds => m_Seconds;

		public Timestep(float seconds)
		{
			m_Seconds = seconds;
		}

		public static implicit operator float(in Timestep ts) => ts.Seconds;

		private readonly float m_Seconds;
	}
}
