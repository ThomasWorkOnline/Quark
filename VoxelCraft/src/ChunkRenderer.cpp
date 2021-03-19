#include "ChunkRenderer.h"

struct RendererData
{
	glm::ivec2 SubTextureSize = { 16, 16 };

	Entropy::Ref<Entropy::Shader> Shader;
	Entropy::Ref<Entropy::Texture2D> Texture;
};

static RendererData s_Data;
static std::unordered_map<BlockId, BlockProperties> s_BlockProperties;

const std::unordered_map<BlockId, BlockProperties>& ChunkRenderer::GetBlockProperties()
{
	return s_BlockProperties;
}

void ChunkRenderer::Initialize()
{
	s_Data.Shader = Entropy::Shader::Create("assets/shaders/default.glsl");
	s_Data.Texture = Entropy::Texture2D::Create("assets/textures/sprite_sheet.png");

	s_BlockProperties = {
		{ BlockId::Air,           { true,  { s_Data.Texture, { 0, 0 }, s_Data.SubTextureSize }, "" } },
		{ BlockId::Bedrock,       { false, { s_Data.Texture, { 0, 0 }, s_Data.SubTextureSize }, "assets/sounds/break_stone.mp3" } },
		{ BlockId::Dirt,          { false, { s_Data.Texture, { 1, 0 }, s_Data.SubTextureSize }, "assets/sounds/break_dirt.mp3"  } },
		{ BlockId::GrassBlock,    { false, { s_Data.Texture, { 2, 0 }, s_Data.SubTextureSize }, "assets/sounds/break_grass.mp3" } },
		{ BlockId::Stone,         { false, { s_Data.Texture, { 0, 1 }, s_Data.SubTextureSize }, "assets/sounds/break_stone.mp3" } },
		{ BlockId::Cobblestone,   { false, { s_Data.Texture, { 1, 1 }, s_Data.SubTextureSize }, "assets/sounds/break_stone.mp3" } }
	};
}

void ChunkRenderer::Shutdown()
{

}

void ChunkRenderer::SubmitChunk(const Chunk& chunk)
{
	Entropy::Renderer::Submit(s_Data.Shader, s_Data.Texture, chunk.GetVertexArray());
}
