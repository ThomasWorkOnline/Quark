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

		static const BlockProperties& GetBlockProperties(Block type);
		static const MeshProperties& GetMeshProperties(BlockModel model);
		static const HitBox& GetBlockHitbox(BlockModel model);
		static const HitBox& GetEntityHitbox(EntityModel model);

		static const Quark::Ref<Quark::Shader>& GetShader(const std::string& name) { return s_ShaderLibrary.Get(name); }
		static const Quark::Ref<Quark::Texture2D>& GetTexture() { return s_Texture; }
		static const Quark::Ref<Quark::VertexArray>& GetCrosshairVertexArray() { return s_CrosshairVertexArray; }
		static const Quark::BufferLayout& GetBufferLayout() { return s_BufferLayout; }

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
