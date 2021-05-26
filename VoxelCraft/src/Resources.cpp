#include "Resources.h"

Quark::Ref<Quark::Shader> Resources::s_Shader;
Quark::Ref<Quark::Texture2D> Resources::s_Texture;
std::unordered_map<Blocks, BlockProperties> Resources::s_BlockProperties;

void Resources::Initialize()
{
	s_Shader = Quark::Shader::Create("assets/shaders/default.glsl");
	s_Texture = Quark::Texture2D::Create("assets/textures/sprite_sheet.png");

	s_BlockProperties = {
		{ Blocks::Air,			BlockProperties::Create({ s_Texture, { 0, 0 }, SubTextureSize }, true, "") },
		{ Blocks::Bedrock,		BlockProperties::Create({ s_Texture, { 0, 0 }, SubTextureSize }, false, "assets/sounds/break_stone.mp3") },
		{ Blocks::Dirt,			BlockProperties::Create({ s_Texture, { 1, 0 }, SubTextureSize }, false, "assets/sounds/break_dirt.mp3") },
		{ Blocks::GrassBlock,	BlockProperties::Create({ s_Texture, { 2, 0 }, SubTextureSize }, { s_Texture, { 1, 0 }, SubTextureSize }, { s_Texture, { 2, 1 }, SubTextureSize }, false, "assets/sounds/break_grass.mp3") },
		{ Blocks::Stone,		BlockProperties::Create({ s_Texture, { 0, 1 }, SubTextureSize }, false, "assets/sounds/break_stone.mp3") },
		{ Blocks::Cobblestone,	BlockProperties::Create({ s_Texture, { 1, 1 }, SubTextureSize }, false, "assets/sounds/break_stone.mp3") },
	};
}
