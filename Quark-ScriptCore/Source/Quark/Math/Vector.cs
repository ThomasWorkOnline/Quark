namespace Quark
{
	public struct Vector2
	{
		public float X, Y;
		public float U => X;
		public float V => Y;

		public static Vector2 Zero => new Vector2();

		public Vector2(float x, float y)
		{
			X = x;
			Y = y;
		}

		public override string ToString()
		{
			return $"({X:F2}, {Y:F2})";
		}

		public static Vector2 operator +(in Vector2 a, in Vector2 b) => new Vector2(a.X + b.X, a.Y + b.Y);
		public static Vector2 operator -(in Vector2 a, in Vector2 b) => new Vector2(a.X - b.X, a.Y - b.Y);
		public static Vector2 operator *(in Vector2 a, in Vector2 b) => new Vector2(a.X * b.X, a.Y * b.Y);
		public static Vector2 operator /(in Vector2 a, in Vector2 b) => new Vector2(a.X / b.X, a.Y / b.Y);
		public static Vector2 operator *(in Vector2 v, float scalar) => new Vector2(v.X * scalar, v.Y * scalar);
		public static Vector2 operator /(in Vector2 v, float scalar) => new Vector2(v.X / scalar, v.Y / scalar);
		public static Vector2 operator *(float scalar, in Vector2 v) => (v * scalar);
		public static Vector2 operator -(in Vector2 v) => (-v);
	}

	public struct Vector3
	{
		public float X, Y, Z;
		public float R => X;
		public float G => Y;
		public float B => Z;

		public static Vector3 Zero => new Vector3();

		public Vector3(float x, float y, float z)
		{
			X = x;
			Y = y;
			Z = z;
		}

		public override string ToString()
		{
			return $"({X:F2}, {Y:F2}, {Z:F2})";
		}

		public static Vector3 operator +(in Vector3 a, in Vector3 b) => new Vector3(a.X + b.X, a.Y + b.Y, a.Z + b.Z);
		public static Vector3 operator -(in Vector3 a, in Vector3 b) => new Vector3(a.X - b.X, a.Y - b.Y, a.Z - b.Z);
		public static Vector3 operator *(in Vector3 a, in Vector3 b) => new Vector3(a.X * b.X, a.Y * b.Y, a.Z * b.Z);
		public static Vector3 operator /(in Vector3 a, in Vector3 b) => new Vector3(a.X / b.X, a.Y / b.Y, a.Z / b.Z);
		public static Vector3 operator *(in Vector3 v, float scalar) => new Vector3(v.X * scalar, v.Y * scalar, v.Z * scalar);
		public static Vector3 operator /(in Vector3 v, float scalar) => new Vector3(v.X / scalar, v.Y / scalar, v.Z / scalar);
		public static Vector3 operator *(float scalar, in Vector3 v) => (v * scalar);
		public static Vector3 operator -(in Vector3 v) => (-v);
	}

	public struct Vector4
	{
		public float X, Y, Z, W;
		public float R => X;
		public float G => Y;
		public float B => Z;
		public float A => W;

		public static Vector4 Zero => new Vector4();

		public Vector4(float x, float y, float z, float w)
		{
			X = x;
			Y = y;
			Z = z;
			W = w;
		}

		public override string ToString()
		{
			return $"({X:F2}, {Y:F2}, {Z:F2}, {W:F2})";
		}

		public static Vector4 operator +(in Vector4 a, in Vector4 b) => new Vector4(a.X + b.X, a.Y + b.Y, a.Z + b.Z, a.W + b.W);
		public static Vector4 operator -(in Vector4 a, in Vector4 b) => new Vector4(a.X - b.X, a.Y - b.Y, a.Z - b.Z, a.W - b.W);
		public static Vector4 operator *(in Vector4 a, in Vector4 b) => new Vector4(a.X * b.X, a.Y * b.Y, a.Z * b.Z, a.W * b.W);
		public static Vector4 operator /(in Vector4 a, in Vector4 b) => new Vector4(a.X / b.X, a.Y / b.Y, a.Z / b.Z, a.W / b.W);
		public static Vector4 operator *(in Vector4 v, float scalar) => new Vector4(v.X * scalar, v.Y * scalar, v.Z * scalar, v.W * scalar);
		public static Vector4 operator /(in Vector4 v, float scalar) => new Vector4(v.X / scalar, v.Y / scalar, v.Z / scalar, v.W / scalar);
		public static Vector4 operator *(float scalar, in Vector4 v) => (v * scalar);
		public static Vector4 operator -(in Vector4 v) => (-v);
	}
}
