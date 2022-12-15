#include "qkpch.h"
#include "SceneSerializer.h"

#include "NativeScriptEntity.h"

#include <type_traits>

namespace Quark {

	//
	// FORMAT:
	//     ["qscn"]     Format Header
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

	static const uint64_t s_SceneVersion = typeid(AllComponents).hash_code();

	template<typename Component>
	static void AddEntityComponent(Entity entity, FILE* in)
	{
		auto component = DeserializeComponent<Component>(in);
		entity.AddComponent<Component>(component);
	}

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
				uint32_t type = (uint32_t)Component::GetStaticType();
				std::fwrite(&type, sizeof(uint32_t), 1, out);

				SerializeComponent<Component>(entity.GetComponent<Component>(), out);
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
			uint32_t componentType;
			std::fread(&componentType, sizeof(uint32_t), 1, in);

			// TODO: runtime validation
			ComponentType type = (ComponentType)componentType;

			switch (type)
			{
				case ComponentType::CameraComponent: AddEntityComponent<CameraComponent>(entity, in); break;
				case ComponentType::TagComponent: AddEntityComponent<TagComponent>(entity, in); break;
				case ComponentType::Transform3DComponent: AddEntityComponent<Transform3DComponent>(entity, in); break;
				case ComponentType::PhysicsComponent: AddEntityComponent<PhysicsComponent>(entity, in); break;
				case ComponentType::StaticMeshComponent: AddEntityComponent<StaticMeshComponent>(entity, in); break;
				case ComponentType::SpriteRendererComponent: AddEntityComponent<SpriteRendererComponent>(entity, in); break;
				case ComponentType::TexturedSpriteRendererComponent: AddEntityComponent<TexturedSpriteRendererComponent>(entity, in); break;
				case ComponentType::TextRendererComponent: AddEntityComponent<TextRendererComponent>(entity, in); break;
				case ComponentType::NativeScriptComponent: AddEntityComponent<NativeScriptComponent>(entity, in); break;
			}
		}
	}

	RuntimeSceneSerializer::RuntimeSceneSerializer(std::string_view scenePath)
		: m_SceneFile(std::fopen(scenePath.data(), "wb"))
	{
		Verify(m_SceneFile, "Could not open '{0}'!", scenePath);
	}

	RuntimeSceneSerializer::~RuntimeSceneSerializer()
	{
		std::fclose(m_SceneFile);
	}

	void RuntimeSceneSerializer::Serialize(Scene* scene)
	{
		QK_PROFILE_FUNCTION();

		// ["qscn"] Format header
		std::fwrite("qscn", sizeof(char), 4, m_SceneFile);

		// [uint64_t] SceneVersion
		std::fwrite(&s_SceneVersion, sizeof(s_SceneVersion), 1, m_SceneFile);

		// [uint32_t] EntityCount
		uint32_t entityCount = (uint32_t)scene->GetRegistry().size();
		std::fwrite(&entityCount, sizeof(uint32_t), 1, m_SceneFile);

		scene->GetRegistry().each([&](entt::entity e)
		{
			Entity entity = { e, scene };
			SerializeComponents(AllComponents{}, entity, m_SceneFile);
		});

		std::fflush(m_SceneFile);
	}

	RuntimeSceneDeserializer::RuntimeSceneDeserializer(std::string_view scenePath)
		: m_SceneFile(std::fopen(scenePath.data(), "rb"))
	{
		Verify(m_SceneFile, "No given scene '{0}' exists!", scenePath);
	}

	RuntimeSceneDeserializer::~RuntimeSceneDeserializer()
	{
		std::fclose(m_SceneFile);
	}

	void RuntimeSceneDeserializer::Deserialize(Scene* scene)
	{
		QK_PROFILE_FUNCTION();

		std::fseek(m_SceneFile, 0, SEEK_END);
		size_t size = std::ftell(m_SceneFile);
		std::fseek(m_SceneFile, 0, SEEK_SET);

		// ["qscn"] Format header
		char header[4];
		std::fread(header, sizeof(char), 4, m_SceneFile);

		if (std::strncmp(header, "qscn", 4) != 0)
		{
			QK_CORE_ERROR("Scene could not be deserialized correctly!");
			return;
		}

		// [uint64_t] SceneVersion
		uint64_t sceneVersion;
		std::fread(&sceneVersion, sizeof(s_SceneVersion), 1, m_SceneFile);

		if (sceneVersion != s_SceneVersion)
		{
			QK_CORE_ERROR("Scene could not be deserialized correctly!");
			return; // Invalid scene version
		}

		// [uint32_t] EntityCount
		uint32_t entityCount;
		std::fread(&entityCount, sizeof(uint32_t), 1, m_SceneFile);

		for (uint32_t entityIndex = 0; entityIndex < entityCount; entityIndex++)
		{
			Entity newEntity = scene->CreateEntity();
			DeserializeComponents(newEntity, m_SceneFile);
		}
	}
}
