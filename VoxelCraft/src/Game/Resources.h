#pragma once

#include <Quark.h>

#include <unordered_map>

#include "Models.h"
#include "../Blocks/Block.h"
#include "../Blocks/BlockProperties.h"
#include "../Rendering/MeshProperties.h"

namespace VoxelCraft {

	class Resources
	{
	public:
		static void Initialize();

		static const BlockProperties& GetBlockProperties(Block type)
		{
			auto it = s_BlockProperties.find(type.ID);
			if (it != s_BlockProperties.end())
			{
				return it->second;
			}

			std::cout << "Type was: " << (int32_t)type.ID;
			QK_FATAL("Block type supplied does not exist");
		}

		static const MeshProperties& GetMeshProperties(BlockModel model)
		{
			auto it = s_MeshProperties.find(model);
			if (it != s_MeshProperties.end())
			{
				return it->second;
			}

			QK_FATAL("Block model supplied does not exist");
		}

		static const HitBox& GetBlockHitbox(BlockModel model)
		{
			auto it = s_BlockHitboxes.find(model);
			if (it != s_BlockHitboxes.end())
			{
				return it->second;
			}

			QK_FATAL("Block model supplied does not exist");
		}

		static const HitBox& GetEntityHitbox(EntityModel model)
		{
			auto it = s_EntityHitboxes.find(model);
			if (it != s_EntityHitboxes.end())
			{
				return it->second;
			}

			QK_FATAL("Entity model supplied does not exist");
		}

		static const Quark::Ref<Quark::Shader>& GetShader(const std::string& name)
		{
			return s_ShaderLibrary.Get(name);
		}

		static const Quark::Ref<Quark::Texture2D>& GetTexture()
		{
			return s_Texture;
		}

		static const Quark::Ref<Quark::VertexArray>& GetCrosshairVertexArray()
		{
			return s_CrosshairVertexArray;
		}

		static const Quark::BufferLayout& GetBufferLayout()
		{
			return s_BufferLayout;
		}

	private:
		static constexpr glm::ivec2 SubTextureSize = { 16, 16 };

		// TODO: load from json
		static std::unordered_map<BlockID, BlockProperties> s_BlockProperties;
		static std::unordered_map<BlockModel, MeshProperties> s_MeshProperties;
		static std::unordered_map<BlockModel, HitBox> s_BlockHitboxes;
		static std::unordered_map<EntityModel, HitBox> s_EntityHitboxes;

		static Quark::Ref<Quark::VertexArray> s_CrosshairVertexArray;

		static Quark::ShaderLibrary s_ShaderLibrary;

		static Quark::Ref<Quark::Texture2D> s_Texture;

		static const Quark::BufferLayout s_BufferLayout;
		static const Quark::BufferLayout s_CrosshairBufferLayout;
	};
}
