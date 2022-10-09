#include "qkpch.h"
#include "SceneSerializer.h"

#include "NativeScriptEntity.h"

#include <type_traits>

namespace Quark {

	//
	// FORMAT:
	//     [uint64_t]   SceneVersion
	//     [uint32_t]   EntityCount
	//     
	//     // Entities Data
	//     [
	//         // Entity
	//         [uint32_t]   ComponentCount
	// 
	//         // Components Data
	//         [
	//             [uint32_t]   ComponentType
	//             [void*]      ComponentData
	// 	       
	//             ...
	//         ]
	//
	//         ...
	//     ]
	//

#define DESERIALIZE_COMPONENT(type, in, entity)                  \
	case ComponentType::type:                                    \
		SceneSerializer::DeserializeComponent<type>(             \
			in,                                                  \
			entity);                                             \
		break

	static const uint64_t s_SceneVersion = typeid(AllComponents).hash_code();
	

	template<typename... Component>
	static uint32_t GetSerializableComponentsCount(ComponentGroup<Component...>, Entity entity)
	{
		uint32_t count = 0;
		(..., [&]()
		{
			if (entity.HasComponent<Component>())
				count++;
		}());

		return count;
	}

	template<typename... Component>
	static void SerializeComponents(ComponentGroup<Component...>, Entity entity, FILE* out)
	{
		// [uint64_t] ComponentCount
		uint32_t componentCount = GetSerializableComponentsCount(AllComponents{}, entity);
		std::fwrite(&componentCount, sizeof(uint32_t), 1, out);

		(..., [&]()
		{
			if (entity.HasComponent<Component>())
			{
				// [uint64_t] ComponentType
				ComponentType type = Component::GetStaticType();
				std::fwrite(&type, sizeof(uint64_t), 1, out);

				SceneSerializer::SerializeComponent<Component>(entity.GetComponent<Component>(), out);
			}
		}());
	}

	static void DeserializeComponents(Entity entity, FILE* in)
	{
		// [uint64_t] ComponentCount
		uint32_t componentCount;
		std::fread(&componentCount, sizeof(uint32_t), 1, in);

		// Read components
		for (uint32_t componentIndex = 0; componentIndex < componentCount; componentIndex++)
		{
			// [uint64_t] ComponentType
			uint64_t componentType;
			std::fread(&componentType, sizeof(uint64_t), 1, in);

			// TODO: runtime validation
			ComponentType type = (ComponentType)componentType;
			switch (type)
			{
				DESERIALIZE_COMPONENT(CameraComponent,                 in, entity);
				DESERIALIZE_COMPONENT(TagComponent,                    in, entity);
				DESERIALIZE_COMPONENT(Transform3DComponent,            in, entity);
				DESERIALIZE_COMPONENT(PhysicsComponent,                in, entity);
				DESERIALIZE_COMPONENT(MeshComponent,                   in, entity);
				DESERIALIZE_COMPONENT(SpriteRendererComponent,         in, entity);
				DESERIALIZE_COMPONENT(TexturedSpriteRendererComponent, in, entity);
				DESERIALIZE_COMPONENT(TextRendererComponent,           in, entity);
				DESERIALIZE_COMPONENT(NativeScriptComponent,           in, entity);

				// TODO: make this work for custom components
			}
		}
	}

	void SceneSerializer::SerializeRuntime(Scene* scene, std::string_view sceneName)
	{
		QK_PROFILE_FUNCTION();

		FILE* out = std::fopen(sceneName.data(), "wb");

		if (!out)
			return;

		// [uint64_t] SceneVersion
		std::fwrite(&s_SceneVersion, sizeof(s_SceneVersion), 1, out);

		// [uint32_t] EntityCount
		uint32_t entityCount = (uint32_t)scene->GetRegistry().size();
		std::fwrite(&entityCount, sizeof(uint32_t), 1, out);

		scene->GetRegistry().each([&](entt::entity e)
		{
			Entity entity = { e, scene };
			SerializeComponents(AllComponents{}, entity, out);
		});

		std::fclose(out);
	}

	void SceneSerializer::DeserializeRuntime(Scene* scene, std::string_view sceneName)
	{
		QK_PROFILE_FUNCTION();

		FILE* in = std::fopen(sceneName.data(), "rb");

		if (!in)
		{
			QK_CORE_ERROR("No given scene '{0}' exists!", sceneName);
			return;
		}

		std::fseek(in, 0, SEEK_END);
		size_t size = std::ftell(in);
		std::fseek(in, 0, SEEK_SET);

		// [uint64_t] SceneVersion
		uint64_t sceneVersion;
		std::fread(&sceneVersion, sizeof(s_SceneVersion), 1, in);

		if (sceneVersion != s_SceneVersion)
		{
			QK_CORE_ERROR("'{0}' could not be deserialized correctly!", sceneName);
			std::fclose(in);
			return; // Invalid scene version
		}

		// [uint32_t] EntityCount
		uint32_t entityCount;
		std::fread(&entityCount, sizeof(uint32_t), 1, in);

		for (uint32_t entityIndex = 0; entityIndex < entityCount; entityIndex++)
		{
			Entity newEntity = scene->CreateEntity();
			DeserializeComponents(newEntity, in);
		}

		std::fclose(in);
	}
}
