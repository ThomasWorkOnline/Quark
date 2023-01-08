namespace Quark
{
	public abstract class Component
	{
		public Entity Entity { get; internal set; }
	}

	public class Transform3DComponent : Component
	{
		public Vector3 Position
		{
			get
			{
				InternalCalls.Transform3DComponent_GetPosition(Entity.Handle, out Vector3 position);
				return position;
			}
			set
			{
				InternalCalls.Transform3DComponent_SetPosition(Entity.Handle, in value);
			}
		}

		public Vector3 Scale
		{
			get
			{
				InternalCalls.Transform3DComponent_GetScale(Entity.Handle, out Vector3 scale);
				return scale;
			}
			set
			{
				InternalCalls.Transform3DComponent_SetScale(Entity.Handle, in value);
			}
		}

		public Quaternion Orientation
		{
			get
			{
				InternalCalls.Transform3DComponent_GetOrientation(Entity.Handle, out Quaternion orientation);
				return orientation;
			}
			set
			{
				InternalCalls.Transform3DComponent_SetOrientation(Entity.Handle, in value);
			}
		}
	}

	public class PhysicsComponent : Component
	{
		public Vector3 Velocity
		{
			get
			{
				InternalCalls.PhysicsComponent_GetVelocity(Entity.Handle, out Vector3 velocity);
				return velocity;
			}
			set
			{
				InternalCalls.PhysicsComponent_SetVelocity(Entity.Handle, in value);
			}
		}

		public Vector3 AngularVelocity
		{
			get
			{
				InternalCalls.PhysicsComponent_GetAngularVelocity(Entity.Handle, out Vector3 angularVelocity);
				return angularVelocity;
			}
			set
			{
				InternalCalls.PhysicsComponent_SetAngularVelocity(Entity.Handle, in value);
			}
		}

		public float DragCoefficient
		{
			get
			{
				return InternalCalls.PhysicsComponent_GetDragCoefficient(Entity.Handle);
			}
			set
			{
				InternalCalls.PhysicsComponent_SetDragCoefficient(Entity.Handle, value);
			}
		}

		public float Mass
		{
			get
			{
				return InternalCalls.PhysicsComponent_GetMass(Entity.Handle);
			}
			set
			{
				InternalCalls.PhysicsComponent_SetMass(Entity.Handle, value);
			}
		}

		public void ApplyForce(in Force force) => ApplyForce(force.Magnitude);
		public void ApplyForce(in Vector3 forceMagnitude) => InternalCalls.PhysicsComponent_ApplyForce(Entity.Handle, forceMagnitude);
	}
}
