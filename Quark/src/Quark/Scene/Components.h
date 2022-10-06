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

	enum class ComponentType : uint32_t
	{
		CameraComponent = 0U,
		TagComponent,
		Transform3DComponent,
		PhysicsComponent,
		MeshComponent,
		SpriteRendererComponent,
		TexturedSpriteRendererComponent,
		TextRendererComponent,
		NativeScriptComponent
	};

#define COMPONENT_TYPE(type) static ComponentType GetStaticType() { return ComponentType::type; }

	struct CameraComponent
	{
		SceneCamera Camera;

		COMPONENT_TYPE(CameraComponent);
	};

	struct TagComponent
	{
		std::string Name;

		TagComponent() = default;
		TagComponent(std::string_view name);

		COMPONENT_TYPE(TagComponent);
	};

	struct Transform3DComponent
	{
		Vec3 Position;
		Vec3 Scale;
		Quat Orientation;

		Vec3 GetFrontVector() const;
		Vec3 GetRightVector() const;
		Vec3 GetTopVector() const;

		Transform3DComponent& SetFrontVector(const Vec3& direction);
		Transform3DComponent& Rotate(const Quat& quat);
		Transform3DComponent& Rotate(Float angle, const Vec3& axis);

		// Conversion operators
		operator Mat4() const { return GetMatrix(); }
		Mat4 GetMatrix() const;

		Transform3DComponent();
		Transform3DComponent(const Transform3DComponent& other);
		Transform3DComponent(const Vec3& position, const Vec3& scale, const Quat& orientation);

		COMPONENT_TYPE(Transform3DComponent);
	};

	struct PhysicsComponent
	{
		Vec3  Velocity;
		Float Mass      = 1.0f; // In kilograms
		Float DragCoeff = 0.7f;

		void AddForce(const Vec3& force);

		PhysicsComponent();
		PhysicsComponent(const Vec3& velocity);

		COMPONENT_TYPE(PhysicsComponent);
	};

	struct MeshComponent
	{
		Mesh* MeshInstance = nullptr;

		MeshComponent() = default;
		MeshComponent(Mesh* meshInstance)
			: MeshInstance(meshInstance)
		{
		}

		COMPONENT_TYPE(MeshComponent);
	};

	struct SpriteRendererComponent
	{
		Vec4f Color = { 1.0f, 1.0f, 1.0f, 1.0f };

		COMPONENT_TYPE(SpriteRendererComponent);
	};

	struct TexturedSpriteRendererComponent
	{
		Ref<Texture2D> Texture;
		Vec4f          Tint = { 1.0f, 1.0f, 1.0f, 1.0f };

		COMPONENT_TYPE(TexturedSpriteRendererComponent);
	};

	struct TextRendererComponent
	{
		Text Label;

		COMPONENT_TYPE(TextRendererComponent);
	};

	class NativeScriptEntity;

	struct NativeScriptComponent
	{
		NativeScriptEntity* ScriptInstance = nullptr;
		NativeScriptEntity* (*InstanciateScript)();

		// Script methods impl
		void (*OnCreate)(NativeScriptComponent&);
		void (*OnDestroy)(NativeScriptComponent&);
		void (*OnUpdate)(NativeScriptEntity*, Timestep);
		void (*OnEvent)(NativeScriptEntity*, Event&);

		template<typename T>
		inline NativeScriptComponent& Bind()
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
				nsc.ScriptInstance = nullptr;
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

		COMPONENT_TYPE(NativeScriptComponent);
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
