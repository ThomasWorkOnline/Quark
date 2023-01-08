using System;
using System.Runtime.CompilerServices;

namespace Quark
{
	static internal class InternalCalls
	{
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static void NativeLog(string text, Logger.LogLevel logLevel);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static uint CreateEntity();

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static void DestroyEntity(uint entityID);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static bool Entity_HasComponent(uint entityID, Type componentType);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static void Transform3DComponent_GetPosition(uint entityID, out Vector3 position);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static void Transform3DComponent_GetScale(uint entityID, out Vector3 scale);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static void Transform3DComponent_GetOrientation(uint entityID, out Quaternion orientation);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static void Transform3DComponent_SetPosition(uint entityID, in Vector3 position);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static void Transform3DComponent_SetScale(uint entityID, in Vector3 scale);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static void Transform3DComponent_SetOrientation(uint entityID, in Quaternion orientation);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static void PhysicsComponent_GetVelocity(uint entityID, out Vector3 velocity);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static void PhysicsComponent_GetAngularVelocity(uint entityID, out Vector3 angularVelocity);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static float PhysicsComponent_GetDragCoefficient(uint entityID);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static float PhysicsComponent_GetMass(uint entityID);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static void PhysicsComponent_SetVelocity(uint entityID, in Vector3 velocity);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static void PhysicsComponent_SetAngularVelocity(uint entityID, in Vector3 angularVelocity);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static void PhysicsComponent_SetDragCoefficient(uint entityID, float dragCoefficient);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static void PhysicsComponent_SetMass(uint entityID, float mass);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static void PhysicsComponent_ApplyForce(uint entityID, in Vector3 forceMagnitude);
	}
}
