#include "ChunkRenderer.h"

#include "Resources.h"

ChunkRendererStats ChunkRenderer::s_Stats;

void ChunkRenderer::Initialize()
{
	
}

void ChunkRenderer::Shutdown()
{

}

void ChunkRenderer::Submit(const Quark::Ref<Quark::VertexArray>& va)
{
	Quark::Renderer::Submit(Resources::GetShader(), Resources::GetTexture(), va);
	s_Stats.DrawCalls++;
}
