using Quark;

namespace Sandbox
{
	public class Player : Entity
	{
		public float GravityAcceleration = 9.8f; // m/s²

		protected void OnCreate()
		{
			m_Physics = Physics;
			m_Physics.DragCoefficient = 0.1f;
		}

		protected void OnUpdate(Timestep ts)
		{
			ApplyGravity(ts);
		}

		private void ApplyGravity(Timestep ts)
		{
			var gravityAcceleration = new Vector3 { Y = -GravityAcceleration };

			// 1 N = 1 kg·m/s²
			Force newtons = new Force(m_Physics.Mass * gravityAcceleration);
			Force normalizedGravityForce = newtons * ts;

			m_Physics.ApplyForce(normalizedGravityForce);
		}

		private PhysicsComponent m_Physics;
	}
}
