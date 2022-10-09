#pragma once

#include "Components.h"

#include <cstdio>

namespace Quark {

	/////////////////////////////////////////////////////////////////
	// Component template specialization
	//

	template<>
	inline void SceneSerializer::SerializeComponent(const TagComponent& tag, FILE* out)
	{
		Serialize(tag.Name, out);
	}

	template<>
	inline void SceneSerializer::DeserializeComponent<TagComponent>(FILE* in, Entity entity)
	{
		auto& tag = entity.AddComponent<TagComponent>();
		tag.Name = Deserialize<std::string>(in);
	}

	template<>
	inline void SceneSerializer::SerializeComponent(const MeshComponent& msc, FILE* out)
	{
	}

	template<>
	inline void SceneSerializer::DeserializeComponent<MeshComponent>(FILE* in, Entity entity)
	{
	}

	template<>
	inline void SceneSerializer::SerializeComponent(const TexturedSpriteRendererComponent& tsr, FILE* out)
	{
		// TODO: serialize texture asset (UUID)
	}

	template<>
	inline void SceneSerializer::DeserializeComponent<TexturedSpriteRendererComponent>(FILE* in, Entity entity)
	{
		// TODO: deserialize texture asset (UUID)
	}

	template<>
	inline void SceneSerializer::SerializeComponent(const TextRendererComponent& tsr, FILE* out)
	{
		// TODO: implement
	}

	template<>
	inline void SceneSerializer::DeserializeComponent<TextRendererComponent>(FILE* in, Entity entity)
	{
		// TODO: implement
	}

	template<>
	inline void SceneSerializer::SerializeComponent(const NativeScriptComponent& nsc, FILE* out)
	{
		Serialize<void*>(nsc.InstanciateScript, out);
		Serialize<void*>(nsc.OnCreate, out);
		Serialize<void*>(nsc.OnDestroy, out);
		Serialize<void*>(nsc.OnUpdate, out);
		Serialize<void*>(nsc.OnEvent, out);
	}

	template<>
	inline void SceneSerializer::DeserializeComponent<NativeScriptComponent>(FILE* in, Entity entity)
	{
		auto& nsc = entity.AddNativeScript<std::nullptr_t>();

		nsc.InstanciateScript = reinterpret_cast<decltype(nsc.InstanciateScript)>(Deserialize<intptr_t>(in));
		nsc.OnCreate = reinterpret_cast<decltype(nsc.OnCreate)>(Deserialize<intptr_t>(in));
		nsc.OnDestroy = reinterpret_cast<decltype(nsc.OnDestroy)>(Deserialize<intptr_t>(in));
		nsc.OnUpdate = reinterpret_cast<decltype(nsc.OnUpdate)>(Deserialize<intptr_t>(in));
		nsc.OnEvent = reinterpret_cast<decltype(nsc.OnEvent)>(Deserialize<intptr_t>(in));

		entity.GetScene().OnComponentAdded<NativeScriptComponent>(entity, nsc);
	}
}
