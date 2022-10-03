#pragma once

#include "Quark/Core/Core.h"

#include "Quark/UI/Text.h"
#include "Quark/Event/Event.h"
#include "Quark/Renderer/Texture.h"

#include "Mesh.h"
#include "SceneCamera.h"
#include "TypeTraits.h"

#include <type_traits>

namespace Quark {

	struct CameraComponent
	{
		SceneCamera Camera;
	};

	struct TagComponent
	{
		std::string Name;

		TagComponent() = default;
		TagComponent(std::string_view name)
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
		Vec3 Velocity = Vec3(0.0f);

		PhysicsComponent();
		PhysicsComponent(const Vec3& velocity);
	};

	struct MeshComponent
	{
		Mesh* MeshInstance = nullptr;

		MeshComponent() = default;
		MeshComponent(Mesh* meshInstance)
			: MeshInstance(meshInstance)
		{
		}
	};

	struct SpriteRendererComponent
	{
		Vec4f Color = { 1.0f, 1.0f, 1.0f, 1.0f };
	};

	struct TexturedSpriteRendererComponent
	{
		Ref<Texture2D> Texture;
		Vec4f Tint = { 1.0f, 1.0f, 1.0f, 1.0f };
	};

	struct TextRendererComponent
	{
		Text Label;
	};

	class NativeScriptEntity;

	struct NativeScriptComponent
	{
		NativeScriptEntity* ScriptInstance = nullptr;
		NativeScriptEntity* (*InstanciateScript)() = nullptr;

		// Script methods impl
		void (*OnCreate)(NativeScriptComponent&) = nullptr;
		void (*OnDestroy)(NativeScriptComponent&) = nullptr;
		void (*OnUpdate)(NativeScriptEntity*, Timestep) = nullptr;
		void (*OnEvent)(NativeScriptEntity*, Event&) = nullptr;

		template<typename T>
		NativeScriptComponent& Bind()
		{
			InstanciateScript = []()
			{
				return static_cast<NativeScriptEntity*>(new T());
			};

			OnCreate = [](NativeScriptComponent& nsc)
			{
				if constexpr (HasOnCreate<T>::value)
					static_cast<T*>(nsc.ScriptInstance)->OnCreate();
			};

			OnDestroy = [](NativeScriptComponent& nsc)
			{
				if constexpr (HasOnDestroy<T>::value)
					static_cast<T*>(nsc.ScriptInstance)->OnDestroy();

				delete nsc.ScriptInstance;
			};

			OnUpdate = [](NativeScriptEntity* script, Timestep ts)
			{
				if constexpr (HasOnUpdate<T>::value)
					static_cast<T*>(script)->OnUpdate(ts);
			};

			OnEvent = [](NativeScriptEntity* script, Event& e)
			{
				if constexpr (HasOnEvent<T>::value)
					static_cast<T*>(script)->OnEvent(e);
			};

			return *this;
		}
	};

	template<typename... Component>
	struct ComponentGroup
	{
		template<typename Func>
		constexpr void Each(Func&& func)
		{
			(..., func(Component{}));
		}
	};

	using AllComponents = ComponentGroup<
		CameraComponent, TagComponent,
		Transform3DComponent, PhysicsComponent,
		MeshComponent, SpriteRendererComponent, TexturedSpriteRendererComponent, TextRendererComponent,
		NativeScriptComponent
	>;
}
