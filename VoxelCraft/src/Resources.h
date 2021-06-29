#pragma once

#include <Quark.h>

#include <unordered_map>

#include "Block.h"
#include "BlockProperties.h"
#include "MeshModel.h"
#include "MeshProperties.h"

class Resources
{
public:
	static void Initialize();

	static const BlockProperties& GetBlockProperties(Block type)
	{
		auto it = s_BlockProperties.find(type);
		if (it != s_BlockProperties.end())
		{
			return it->second;
		}

		QK_FATAL("Block type supplied does not exist");
	}

	static const MeshProperties& GetMeshProperties(MeshModel model)
	{
		auto it = s_MeshModels.find(model);
		if (it != s_MeshModels.end())
		{
			return it->second;
		}

		QK_FATAL("Mesh model supplied does not exist");
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

	static const HitBox& GetPlayerHitbox()
	{
		return s_PlayerHitbox;
	}

private:
	static constexpr glm::ivec2 SubTextureSize = { 16, 16 };

	// TODO: load from json
	static std::unordered_map<Block, BlockProperties> s_BlockProperties;
	static std::unordered_map<MeshModel, MeshProperties> s_MeshModels;

	static Quark::Ref<Quark::VertexArray> s_CrosshairVertexArray;

	static Quark::ShaderLibrary s_ShaderLibrary;

	static Quark::Ref<Quark::Texture2D> s_Texture;

	static const Quark::BufferLayout s_BufferLayout;
	static const Quark::BufferLayout s_CrosshairBufferLayout;

	static const HitBox s_PlayerHitbox;
};
