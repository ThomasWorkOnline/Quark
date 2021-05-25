#include "ChunkRenderer.h"

struct RendererData
{
	glm::ivec2 SubTextureSize = { 16, 16 };

	Quark::Ref<Quark::Shader> Shader;
	Quark::Ref<Quark::Texture2D> Texture;
};

Quark::RenderStats ChunkRenderer::s_Stats;
static RendererData s_Data;
static std::unordered_map<BlockId, BlockProperties> s_BlockProperties;

const std::unordered_map<BlockId, BlockProperties>& ChunkRenderer::GetBlockProperties()
{
	return s_BlockProperties;
}

void ChunkRenderer::Initialize()
{
	QK_TIME_SCOPE_DEBUG(ChunkRenderer::Initialize);

	s_Data.Shader = Quark::Shader::Create("assets/shaders/default.glsl");
	s_Data.Texture = Quark::Texture2D::Create("assets/textures/sprite_sheet.png");

	s_BlockProperties = {
		{ BlockId::Air,         BlockPropertiesFactory::Create({ s_Data.Texture, { 0, 0 }, s_Data.SubTextureSize }, true, "") },
		{ BlockId::Bedrock,     BlockPropertiesFactory::Create({ s_Data.Texture, { 0, 0 }, s_Data.SubTextureSize }, false, "assets/sounds/break_stone.mp3") },
		{ BlockId::Dirt,		BlockPropertiesFactory::Create({ s_Data.Texture, { 1, 0 }, s_Data.SubTextureSize }, false, "assets/sounds/break_dirt.mp3") },
		{ BlockId::GrassBlock,	BlockPropertiesFactory::Create({ s_Data.Texture, { 2, 0 }, s_Data.SubTextureSize }, { s_Data.Texture, { 1, 0 }, s_Data.SubTextureSize }, { s_Data.Texture, { 2, 1 }, s_Data.SubTextureSize }, false, "assets/sounds/break_grass.mp3") },
		{ BlockId::Stone,		BlockPropertiesFactory::Create({ s_Data.Texture, { 0, 1 }, s_Data.SubTextureSize }, false, "assets/sounds/break_stone.mp3") },
		{ BlockId::Cobblestone,	BlockPropertiesFactory::Create({ s_Data.Texture, { 1, 1 }, s_Data.SubTextureSize }, false, "assets/sounds/break_stone.mp3") },
	};
}

void ChunkRenderer::Shutdown()
{

}

void ChunkRenderer::SubmitChunk(Chunk* chunk)
{
	Quark::Renderer::Submit(s_Data.Shader, s_Data.Texture, chunk->GetVertexArray());
	s_Stats.DrawCalls++;
}
