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
		return s_BlockProperties.at(type);
	}

	static const MeshProperties& GetMeshProperties(MeshModel model)
	{
		return s_MeshModels.at(model);
	}

	static const Quark::Ref<Quark::Shader>& GetShader()
	{
		return s_Shader;
	}

	static const Quark::Ref<Quark::Shader>& GetDebugShader()
	{
		return s_DebugShader;
	}

	static const Quark::Ref<Quark::Texture2D>& GetTexture()
	{
		return s_Texture;
	}

private:
	static constexpr glm::ivec2 SubTextureSize = { 16, 16 };

	// TODO: load from json
	static std::unordered_map<Block, BlockProperties> s_BlockProperties;
	static std::unordered_map<MeshModel, MeshProperties> s_MeshModels;

	static Quark::Ref<Quark::Shader> s_Shader;
	static Quark::Ref<Quark::Shader> s_DebugShader;
	static Quark::Ref<Quark::Texture2D> s_Texture;
};
