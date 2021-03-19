#include "ChunkRenderer.h"

#include "Block.h"

static RendererData s_Data;

const RendererData& ChunkRenderer::GetData()
{
	return s_Data;
}

void ChunkRenderer::Initialize()
{
	s_Data.Shader = Entropy::Shader::Create("assets/shaders/default.glsl");
	s_Data.Texture = Entropy::Texture2D::Create("assets/textures/spriteSheet.png");
}

void ChunkRenderer::Shutdown()
{

}

void ChunkRenderer::SubmitChunk(const Chunk& chunk)
{
	Entropy::Renderer::Submit(s_Data.Shader, s_Data.Texture, chunk.GetVertexArray());
}
