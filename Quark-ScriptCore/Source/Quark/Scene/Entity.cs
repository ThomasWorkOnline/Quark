using System;

namespace Quark
{
	public class Entity
	{
		public Entity()
		{
		}

		public Transform3DComponent Transform => GetComponent<Transform3DComponent>();
		public PhysicsComponent Physics => GetComponent<PhysicsComponent>();

		public bool HasComponent<T>() where T : Component
		{
			Type componentType = typeof(T);
			return InternalCalls.Entity_HasComponent(Handle, componentType);
		}

		public T GetComponent<T>() where T : Component, new()
		{
			if (!HasComponent<T>())
				return null;

			return new T() { Entity = this };
		}

		internal uint Handle => m_EntityHandle;

		private readonly uint m_EntityHandle = 0u;
	}
}
