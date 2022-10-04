#pragma once

#include "Scene.h"
#include "Entity.h"
#include "Components.h"

#include <cstdio>

namespace Quark {

	class SceneSerializer
	{
	public:
		void SerializeRuntime(Scene* scene, std::string_view sceneName);
		void DeserializeRuntime(Scene* scene, std::string_view sceneName);
	};

	template<typename T>
	inline void Serialize(const T&, FILE*)
	{
		static_assert(sizeof(T) == 0, "No serialize specialization was defined for type T.");
	}

	template<typename T>
	inline auto Deserialize(FILE*) -> T
	{
		static_assert(sizeof(T) == 0, "No deserialize specialization was defined for type T.");
	}

	///////////////////////////////////////////////////////////////////////
	// Default impl for component serialization/deserialization
	//

	template<typename Component>
	inline void SerializeComponent(const Component& component, FILE* out)
	{
		std::fwrite(&component, sizeof(Component), 1, out);
	}

	template<typename Component>
	inline void DeserializeComponent(FILE* in, Entity entity)
	{
		auto& c = entity.AddComponent<Component>();
		std::fread(&c, sizeof(Component), 1, in);
	}
}
