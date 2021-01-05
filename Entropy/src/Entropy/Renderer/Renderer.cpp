#include "Renderer.h"

#include "../Core/Core.h"

#include "RenderingAPI.h"

namespace Entropy {

	Renderer::SceneData Renderer::s_SceneData;

	struct RendererData
	{

	};

	static RendererData s_Data;

	void Renderer::Init()
	{
		RenderCommand::Init();
	}

	void Renderer::Dispose()
	{

	}

	void Renderer::BeginScene(const Camera& camera)
	{
		s_SceneData.ViewProjectionMatrix = camera.GetViewProjectionMatrix();
		s_SceneData.CameraPosition = camera.GetPosition();

		StartBatch();
	}

	void Renderer::EndScene()
	{
		Flush();
	}

	void Renderer::Flush()
	{
		// TODO: Render
	}

	void Renderer::Submit(const Ref<Shader>& shader, const Ref<VertexArray>& va, const glm::mat4& transform)
	{
		shader->Attach();
		shader->SetMat4("u_ViewProjection", s_SceneData.ViewProjectionMatrix);
		shader->SetFloat3("u_CameraPosition", s_SceneData.CameraPosition);
		shader->SetMat4("u_Model", transform);

		// TODO: append this to batch
		va->Attach();
		RenderCommand::Draw(va);
	}

	void Renderer::StartBatch()
	{

	}

	void Renderer::NextBatch()
	{

	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		RenderCommand::SetViewport(0, 0, width, height);
	}
}
