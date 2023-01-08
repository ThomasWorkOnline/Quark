#include "qkpch.h"
#include "InternalCalls.h"

#include "ScriptRuntime.h"

#include "Quark/Scene/Scene.h"
#include "Quark/Scene/Entity.h"

#include <mono/jit/jit.h>
#include <mono/metadata/reflection.h>

#define QK_ADD_INTERNAL_CALL(x) mono_add_internal_call("Quark.InternalCalls::" #x, x)

namespace Quark {

	enum class LogLevel : uint8_t
	{
		Trace = 0,
		Info,
		Warn,
		Error,
		Critical
	};

	using uint = uint32_t;

	namespace InternalCalls {

		static Entity GetEntityByID(uint entityID)
		{
			auto* scene = ScriptRuntime::GetRuntimeContext();
			QK_CORE_ASSERT(scene, "No runtime context has been set!");

			Entity entity = { (entt::entity)entityID, scene };
			QK_CORE_ASSERT(entity, "Entity: {0} does not exist!", (uint32_t)entity);

			return entity;
		}

		static void NativeLog(MonoString* string, LogLevel logLevel)
		{
			char* cStr = mono_string_to_utf8(string);

			switch (logLevel)
			{
				case LogLevel::Trace: 	 QK_TRACE(cStr);
					break;
				case LogLevel::Info:     QK_INFO(cStr);
					break;
				case LogLevel::Warn:     QK_WARN(cStr);
					break;
				case LogLevel::Error:    QK_ERROR(cStr);
					break;
				case LogLevel::Critical: QK_CRITICAL(cStr);
					break;
			}

			mono_free(cStr);
		}

		static uint CreateEntity()
		{
			auto* scene = ScriptRuntime::GetRuntimeContext();
			QK_CORE_ASSERT(scene, "No runtime context has been set!");

			Entity entity = scene->CreateEntity();
			return (uint)entity;
		}

		static void DestroyEntity(uint entityID)
		{
			Entity entity = GetEntityByID(entityID);

			auto* scene = ScriptRuntime::GetRuntimeContext();
			scene->DestroyEntity(entity);
		}

		static bool Entity_HasComponent(uint entityID, MonoReflectionType* componentType)
		{
			Entity entity = GetEntityByID(entityID);
			MonoType* managedType = mono_reflection_type_get_type(componentType);
			return ScriptRuntime::EntityHasComponentType(entity, managedType);
		}

		static void Transform3DComponent_GetPosition(uint entityID, Vec3f* outPosition)
		{
			Entity entity = GetEntityByID(entityID);
			*outPosition = entity.GetComponent<Transform3DComponent>().Position;
		}

		static void Transform3DComponent_GetScale(uint entityID, Vec3f* outScale)
		{
			Entity entity = GetEntityByID(entityID);
			*outScale = entity.GetComponent<Transform3DComponent>().Scale;
		}

		static void Transform3DComponent_GetOrientation(uint entityID, Quatf* outOrientation)
		{
			Entity entity = GetEntityByID(entityID);
			*outOrientation = entity.GetComponent<Transform3DComponent>().Orientation;
		}

		static void Transform3DComponent_SetPosition(uint entityID, Vec3f position)
		{
			Entity entity = GetEntityByID(entityID);
			entity.GetComponent<Transform3DComponent>().Position = position;
		}

		static void Transform3DComponent_SetScale(uint entityID, Vec3f scale)
		{
			Entity entity = GetEntityByID(entityID);
			entity.GetComponent<Transform3DComponent>().Scale = scale;
		}

		static void Transform3DComponent_SetOrientation(uint entityID, Quatf orientation)
		{
			Entity entity = GetEntityByID(entityID);
			entity.GetComponent<Transform3DComponent>().Orientation = orientation;
		}

		static void PhysicsComponent_GetVelocity(uint entityID, Vec3f* outVelocity)
		{
			Entity entity = GetEntityByID(entityID);
			*outVelocity = entity.GetComponent<PhysicsComponent>().Velocity;
		}

		static void PhysicsComponent_GetAngularVelocity(uint entityID, Vec3f* outAngularVelocity)
		{
			Entity entity = GetEntityByID(entityID);
			*outAngularVelocity = entity.GetComponent<PhysicsComponent>().AngularVelocity;
		}

		static float PhysicsComponent_GetDragCoefficient(uint entityID)
		{
			Entity entity = GetEntityByID(entityID);
			return (float)entity.GetComponent<PhysicsComponent>().DragCoeff;
		}

		static float PhysicsComponent_GetMass(uint entityID)
		{
			Entity entity = GetEntityByID(entityID);
			return (float)entity.GetComponent<PhysicsComponent>().Mass;
		}

		static void PhysicsComponent_SetVelocity(uint entityID, Vec3f velocity)
		{
			Entity entity = GetEntityByID(entityID);
			entity.GetComponent<PhysicsComponent>().Velocity = velocity;
		}

		static void PhysicsComponent_SetAngularVelocity(uint entityID, Vec3f angularVelocity)
		{
			Entity entity = GetEntityByID(entityID);
			entity.GetComponent<PhysicsComponent>().AngularVelocity = angularVelocity;
		}

		static void PhysicsComponent_SetDragCoefficient(uint entityID, float dragCoefficient)
		{
			Entity entity = GetEntityByID(entityID);
			entity.GetComponent<PhysicsComponent>().DragCoeff = dragCoefficient;
		}

		static void PhysicsComponent_SetMass(uint entityID, float mass)
		{
			Entity entity = GetEntityByID(entityID);
			entity.GetComponent<PhysicsComponent>().Mass = mass;
		}

		static void PhysicsComponent_ApplyForce(uint entityID, Vec3f force)
		{
			Entity entity = GetEntityByID(entityID);
			entity.GetComponent<PhysicsComponent>().ApplyForce((Vec3)force);
		}
	}

	void InternalCalls::RegisterInternalCalls()
	{
		QK_ADD_INTERNAL_CALL(NativeLog);
		QK_ADD_INTERNAL_CALL(CreateEntity);
		QK_ADD_INTERNAL_CALL(DestroyEntity);
		QK_ADD_INTERNAL_CALL(Entity_HasComponent);

		QK_ADD_INTERNAL_CALL(Transform3DComponent_GetPosition);
		QK_ADD_INTERNAL_CALL(Transform3DComponent_GetScale);
		QK_ADD_INTERNAL_CALL(Transform3DComponent_GetOrientation);
		QK_ADD_INTERNAL_CALL(Transform3DComponent_SetPosition);
		QK_ADD_INTERNAL_CALL(Transform3DComponent_SetScale);
		QK_ADD_INTERNAL_CALL(Transform3DComponent_SetOrientation);

		QK_ADD_INTERNAL_CALL(PhysicsComponent_GetVelocity);
		QK_ADD_INTERNAL_CALL(PhysicsComponent_GetAngularVelocity);
		QK_ADD_INTERNAL_CALL(PhysicsComponent_GetDragCoefficient);
		QK_ADD_INTERNAL_CALL(PhysicsComponent_GetMass);
		QK_ADD_INTERNAL_CALL(PhysicsComponent_SetVelocity);
		QK_ADD_INTERNAL_CALL(PhysicsComponent_SetAngularVelocity);
		QK_ADD_INTERNAL_CALL(PhysicsComponent_SetDragCoefficient);
		QK_ADD_INTERNAL_CALL(PhysicsComponent_SetMass);
		QK_ADD_INTERNAL_CALL(PhysicsComponent_ApplyForce);
	}
}
