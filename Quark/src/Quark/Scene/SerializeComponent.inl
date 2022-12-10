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
	inline void SceneSerializer::SerializeComponent(const StaticMeshComponent& msc, FILE* out)
	{
	}

	template<>
	inline void SceneSerializer::DeserializeComponent<StaticMeshComponent>(FILE* in, Entity entity)
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
		Serialize<const void*>((const void*)nsc.InstanciateScript, out);
		Serialize<const void*>((const void*)nsc.OnCreateImpl, out);
		Serialize<const void*>((const void*)nsc.OnDestroyImpl, out);
		Serialize<const void*>((const void*)nsc.OnUpdateImpl, out);
		Serialize<const void*>((const void*)nsc.OnEventImpl, out);
	}

	template<>
	inline void SceneSerializer::DeserializeComponent<NativeScriptComponent>(FILE* in, Entity entity)
	{
		auto& nsc = entity.AddNativeScript<std::nullptr_t>();

		nsc.InstanciateScript = reinterpret_cast<decltype(nsc.InstanciateScript)>(Deserialize<intptr_t>(in));
		nsc.OnCreateImpl = reinterpret_cast<decltype(nsc.OnCreateImpl)>(Deserialize<intptr_t>(in));
		nsc.OnDestroyImpl = reinterpret_cast<decltype(nsc.OnDestroyImpl)>(Deserialize<intptr_t>(in));
		nsc.OnUpdateImpl = reinterpret_cast<decltype(nsc.OnUpdateImpl)>(Deserialize<intptr_t>(in));
		nsc.OnEventImpl = reinterpret_cast<decltype(nsc.OnEventImpl)>(Deserialize<intptr_t>(in));

		entity.GetScene().OnComponentAdded<NativeScriptComponent>(entity, nsc);
	}
}
