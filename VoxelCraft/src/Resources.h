#pragma once

#include <Quark.h>

#include <unordered_map>

#include "Block.h"
#include "BlockProperties.h"

class Resources
{
public:
	static void Initialize();

	static const std::unordered_map<Block, BlockProperties>& GetBlockProperties()
	{
		return s_BlockProperties;
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

	static Quark::Ref<Quark::Shader> s_Shader;
	static Quark::Ref<Quark::Shader> s_DebugShader;
	static Quark::Ref<Quark::Texture2D> s_Texture;
};
