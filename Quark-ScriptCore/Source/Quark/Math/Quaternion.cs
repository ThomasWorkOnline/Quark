namespace Quark
{
	public struct Quaternion
	{
		public float X, Y, Z, W;

		public override string ToString()
		{
			return $"({X:F2}, {Y:F2}, {Z:F2}, {W:F2})";
		}
	}
}
