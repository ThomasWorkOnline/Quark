#pragma once

#include "Scene.h"
#include "Entity.h"

#include <cstdio>
#include <type_traits>

namespace Quark {

	struct SceneSerializer
	{
		virtual ~SceneSerializer() = default;

		virtual void Serialize(Scene* scene) = 0;
	};

	struct SceneDeserializer
	{
		virtual ~SceneDeserializer() = default;

		virtual void Deserialize(Scene* scene) = 0;
	};

	class RuntimeSceneSerializer : public SceneSerializer
	{
	public:
		RuntimeSceneSerializer(std::string_view scenePath);
		virtual ~RuntimeSceneSerializer() final override;

		virtual void Serialize(Scene* scene) final override;

	private:
		FILE* m_SceneFile;
	};

	class RuntimeSceneDeserializer : public SceneDeserializer
	{
	public:
		RuntimeSceneDeserializer(std::string_view scenePath);
		virtual ~RuntimeSceneDeserializer() final override;

		virtual void Deserialize(Scene* scene) final override;

	private:
		FILE* m_SceneFile;
	};

	///////////////////////////////////////////////////////////////////////
	// Default implementation for component serialization/deserialization
	//
	template<typename Component>
	static inline void SerializeComponent(const Component& component, FILE* out)
	{
		static_assert(sizeof(Component) == 0, "No SerializeComponent specialization was defined for Component");
	}

	template<typename Component>
	static inline auto DeserializeComponent(FILE* in) -> Component
	{
		static_assert(sizeof(Component) == 0, "No DeserializeComponent specialization was defined for Component");
	}
}

#include "SerializeComponent.inl"
