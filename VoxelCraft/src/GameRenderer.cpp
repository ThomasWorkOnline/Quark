#include "GameRenderer.h"

#include "Resources.h"

#define DRAW_CHUNK_ALL_STATUS 0

Quark::Ref<Quark::Shader> GameRenderer::s_ActiveShader;

GameRendererStats GameRenderer::s_Stats;

// Debug
#if DRAW_CHUNK_ALL_STATUS
static Quark::Ref<Quark::Shader> s_Shader;
static const Quark::BufferLayout s_Layout = {
	{ Quark::ShaderDataType::Float3, "a_Position" },
	{ Quark::ShaderDataType::Float3, "a_Normal"   },
	{ Quark::ShaderDataType::Float2, "a_TexCoord" }
};
static Quark::Mesh s_Mesh;

static Quark::Transform3DComponent s_Transform;
#endif

void GameRenderer::Initialize()
{
	s_ActiveShader = Resources::GetShader("default");

#	if DRAW_CHUNK_ALL_STATUS
	s_Shader = Quark::Shader::Create("assets/shaders/default3D.glsl");

	s_Mesh = { s_Layout, "assets/models/arrow.obj" };

	s_Transform.Position = { 0.0f, 65.0f, 0.0f };
	s_Transform.Scale = { 0.4f, 0.2f, 0.2f };
	s_Transform.Orientation = glm::angleAxis(glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
#	endif
}

void GameRenderer::Shutdown()
{

}

void GameRenderer::SwitchShader()
{
	if (s_ActiveShader->GetName() == "default")
	{
		s_ActiveShader = Resources::GetShader("debugMesh");
	}
	else
	{
		s_ActiveShader = Resources::GetShader("default");
	}
}

void GameRenderer::SubmitChunk(const Chunk* chunk)
{
	switch (chunk->GetLoadStatus())
	{
	case Chunk::LoadStatus::Loaded:
	{
		Quark::Renderer::Submit(s_ActiveShader, Resources::GetTexture(), chunk->GetVertexArray());
		s_Stats.DrawCalls++;
		break;
	}
#	if DRAW_CHUNK_ALL_STATUS
	case Chunk::LoadStatus::WorldGenerated:
	{
		auto& coord = chunk->GetCoord();
		s_Transform.Position = { coord.x * (int32_t)ChunkSpecification::Width + 8, 64, coord.y * (int32_t)ChunkSpecification::Depth + 8 };
		s_Transform.Orientation = glm::angleAxis(glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		Quark::Renderer::Submit(s_Shader, s_Mesh.GetVertexArray(), s_Transform);
		s_Stats.DrawCalls++;
		break;
	}
	case Chunk::LoadStatus::Allocated:
	{
		auto& coord = chunk->GetCoord();
		s_Transform.Position = { coord.x * (int32_t)ChunkSpecification::Width + 8, 64, coord.y * (int32_t)ChunkSpecification::Depth + 8 };
		s_Transform.Orientation = glm::angleAxis(glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

		Quark::Renderer::Submit(s_Shader, s_Mesh.GetVertexArray(), s_Transform);
		s_Stats.DrawCalls++;
		break;
	}
#	endif
	}
}

void GameRenderer::DrawUI(uint32_t width, uint32_t height)
{
	// Draw UI
	static const auto& shader = Resources::GetShader("crosshair");

	float aspectRatio = static_cast<float>(width) / height;
	Quark::Renderer::BeginScene(glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, aspectRatio, 1.0f)), glm::mat4(1.0f));

	shader->Attach();
	shader->SetFloat4("u_Color", glm::vec4(1.0f, 1.0f, 1.0f, 0.5f));

	Quark::Renderer::Submit(shader, Resources::GetCrosshairVertexArray());
	Quark::Renderer::EndScene();
}
