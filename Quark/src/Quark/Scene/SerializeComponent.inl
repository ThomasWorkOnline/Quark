#pragma once

#include "Components.h"
#include "Serialization.h"

#include <cstdio>

namespace Quark {

	/////////////////////////////////////////////////////////////////
	// Component template specialization
	//

	template<>
	inline void SerializeComponent(const CameraComponent& cc, FILE* out)
	{
		// TODO: implement
	}

	template<>
	inline auto DeserializeComponent(FILE* in) -> CameraComponent
	{
		// TODO: implement
		return {};
	}

	template<>
	inline void SerializeComponent(const TagComponent& tag, FILE* out)
	{
		Serializer serializer(out);
		serializer.SerializeValue(tag.Name);
	}

	template<>
	inline auto DeserializeComponent(FILE* in) -> TagComponent
	{
		Deserializer deserializer(in);
		TagComponent tag;
		tag.Name = deserializer.DeserializeValue<std::string>();
		return tag;
	}

	template<>
	inline void SerializeComponent(const Transform3DComponent& tc, FILE* out)
	{
		// TODO: implement
	}

	template<>
	inline auto DeserializeComponent(FILE* in) -> Transform3DComponent
	{
		// TODO: implement
		return {};
	}

	template<>
	inline void SerializeComponent(const PhysicsComponent& pc, FILE* out)
	{
		// TODO: implement
	}

	template<>
	inline auto DeserializeComponent(FILE* in) -> PhysicsComponent
	{
		// TODO: implement
		return {};
	}

	template<>
	inline void SerializeComponent(const StaticMeshComponent& msc, FILE* out)
	{
	}

	template<>
	inline auto DeserializeComponent(FILE* in) -> StaticMeshComponent
	{
		return {};
	}

	template<>
	inline void SerializeComponent(const SpriteRendererComponent& src, FILE* out)
	{
	}

	template<>
	inline auto DeserializeComponent(FILE* in) -> SpriteRendererComponent
	{
		return {};
	}

	template<>
	inline void SerializeComponent(const TexturedSpriteRendererComponent& tsr, FILE* out)
	{
		// TODO: serialize texture asset (UUID)
	}

	template<>
	inline auto DeserializeComponent(FILE* in) -> TexturedSpriteRendererComponent
	{
		// TODO: deserialize texture asset (UUID)
		return {};
	}

	template<>
	inline void SerializeComponent(const TextRendererComponent& tsr, FILE* out)
	{
		// TODO: implement
	}

	template<>
	inline auto DeserializeComponent(FILE* in) -> TextRendererComponent
	{
		// TODO: implement
		return {};
	}

	template<>
	inline void SerializeComponent(const ScriptComponent& sc, FILE* out)
	{
		// TODO: implement
	}

	template<>
	inline auto DeserializeComponent(FILE* in) -> ScriptComponent
	{
		// TODO: implement
		return {};
	}

	template<>
	inline void SerializeComponent(const NativeScriptComponent& nsc, FILE* out)
	{
		Serializer serializer(out);
		serializer.SerializeValue<std::uintptr_t>(
			reinterpret_cast<std::uintptr_t>(nsc.InstanciateScript)
		);
		serializer.SerializeValue<std::uintptr_t>(
			reinterpret_cast<std::uintptr_t>(nsc.OnCreate)
		);
		serializer.SerializeValue<std::uintptr_t>(
			reinterpret_cast<std::uintptr_t>(nsc.OnDestroy)
		);
		serializer.SerializeValue<std::uintptr_t>(
			reinterpret_cast<std::uintptr_t>(nsc.OnUpdate)
		);
		serializer.SerializeValue<std::uintptr_t>(
			reinterpret_cast<std::uintptr_t>(nsc.OnEvent)
		);
	}

	template<>
	inline auto DeserializeComponent(FILE* in) -> NativeScriptComponent
	{
		Deserializer deserializer(in);
		NativeScriptComponent nsc;
		nsc.InstanciateScript = reinterpret_cast<decltype(nsc.InstanciateScript)>(
			deserializer.DeserializeValue<std::uintptr_t>()
		);
		nsc.OnCreate = reinterpret_cast<decltype(nsc.OnCreate)>(
			deserializer.DeserializeValue<std::uintptr_t>()
		);
		nsc.OnDestroy = reinterpret_cast<decltype(nsc.OnDestroy)>(
			deserializer.DeserializeValue<std::uintptr_t>()
		);
		nsc.OnUpdate = reinterpret_cast<decltype(nsc.OnUpdate)>(
			deserializer.DeserializeValue<std::uintptr_t>()
		);
		nsc.OnEvent = reinterpret_cast<decltype(nsc.OnEvent)>(
			deserializer.DeserializeValue<std::uintptr_t>()
		);
		return nsc;
	}
}
