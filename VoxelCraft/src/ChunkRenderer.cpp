#include "ChunkRenderer.h"

#include "Resources.h"

ChunkRendererStats ChunkRenderer::s_Stats;

Quark::Ref<Quark::Shader> ChunkRenderer::s_ActiveShader;

void ChunkRenderer::Initialize()
{
	s_ActiveShader = Resources::GetShader();
}

void ChunkRenderer::Shutdown()
{

}

void ChunkRenderer::SwitchShader()
{
	if (s_ActiveShader->GetName() == "default")
	{
		s_ActiveShader = Resources::GetDebugShader();
	}
	else
	{
		s_ActiveShader = Resources::GetShader();
	}
}

void ChunkRenderer::Submit(const Quark::Ref<Quark::VertexArray>& va)
{
	Quark::Renderer::Submit(s_ActiveShader, Resources::GetTexture(), va);
	s_Stats.DrawCalls++;
}
