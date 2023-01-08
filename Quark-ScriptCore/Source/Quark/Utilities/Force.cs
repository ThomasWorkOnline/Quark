namespace Quark
{
	public struct Force
	{
		public Vector3 Magnitude => m_Force;

		public Force(in Vector3 newtonForce)
		{
			m_Force = newtonForce;
		}

		public static Force operator +(in Force a, in Force b) => new Force(a.Magnitude + b.Magnitude);
		public static Force operator -(in Force a, in Force b) => new Force(a.Magnitude - b.Magnitude);
		public static Force operator *(in Force a, float scalar) => new Force(a.Magnitude * scalar);
		public static Force operator /(in Force a, float scalar) => new Force(a.Magnitude / scalar);

		private Vector3 m_Force;
	}
}
