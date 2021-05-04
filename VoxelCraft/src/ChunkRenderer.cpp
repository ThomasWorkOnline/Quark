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

void ChunkRenderer::SubmitChunk(Chunk* chunk)
{
	Quark::Renderer::BeginScene(chunk->GetWorld().GetPlayer().GetCamera().Camera.GetMatrix(),
		chunk->GetWorld().GetPlayer().GetTransform());

	chunk->PushData();
	Quark::Renderer::Submit(s_Data.Shader, s_Data.Texture, chunk->GetVertexArray());
	s_Stats.DrawCalls++;

	Quark::Renderer::EndScene();
}
