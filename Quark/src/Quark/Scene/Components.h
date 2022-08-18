#pragma once

#include "Quark/Core/Core.h"
#include "Quark/Renderer/Texture.h"

#include "Mesh.h"
#include "SceneCamera.h"

namespace Quark {

	struct CameraComponent
	{
		SceneCamera Camera;
	};

	struct TagComponent
	{
		std::string Name;

		TagComponent(const std::string& name)
			: Name(name) {}
	};

	struct Transform3DComponent
	{
		Vec3 Position;
		Vec3 Scale;
		Quat Orientation;

		Transform3DComponent();
		Transform3DComponent(const Transform3DComponent& other);
		Transform3DComponent(const Vec3& position, const Vec3& scale, const Quat& orientation);

		Vec3 GetFrontVector() const;
		Vec3 GetRightVector() const;
		Vec3 GetTopVector() const;

		Transform3DComponent& SetFrontVector(const Vec3& direction);
		Transform3DComponent& Rotate(const Quat& quat);
		Transform3DComponent& Rotate(Float angle, const Vec3& axis);

		// Conversion operators
		operator Mat4() const { return GetMatrix(); }
		Mat4 GetMatrix() const;
	};

	struct PhysicsComponent
	{
		Vec3 Velocity;

		PhysicsComponent();
		PhysicsComponent(const Vec3& velocity);
	};

	struct MeshComponent
	{
		Mesh* MeshInstance = nullptr;

		MeshComponent(Mesh* meshInstance)
			: MeshInstance(meshInstance)
		{
		}
	};

	struct ColoredSpriteRendererComponent
	{
		Vec4f Color = { 1.0f, 1.0f, 1.0f, 1.0f };
	};

	struct TexturedSpriteRendererComponent
	{
		Ref<Texture2D> Texture;
		Vec4f Tint = { 1.0f, 1.0f, 1.0f, 1.0f };
	};

	class Scene;
	class Entity;
	class NativeScriptEntity;

	struct NativeScriptComponent
	{
		Scope<NativeScriptEntity> ScriptInstance;
		NativeScriptEntity* (*InstanciateScript)(Entity) = nullptr;

		template<typename T>
		NativeScriptComponent& Bind()
		{
			static_assert(std::is_base_of_v<NativeScriptEntity, T>,
				"Template argument must be a sub-type of NativeScriptEntity");

			InstanciateScript = [](Entity entity)
			{
				auto* script = static_cast<NativeScriptEntity*>(new T());
				script->m_Entity = entity;
				return script;
			};

			return *this;
		}
	};
}
