#include "Renderer.h"

#include "../Core/Core.h"
#include "../Scene/Scene.h"
#include "../Scene/Entity.h"
#include "../Scene/Components.h"

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

	void Renderer::BeginScene(Entity cameraEntity)
	{
		auto& camera = cameraEntity.GetComponent<CameraComponent>().Camera;
		auto& transform = cameraEntity.GetComponent<TransformComponent>();

		transform.Orientation = glm::normalize(transform.Orientation);
		glm::mat4 rotate = glm::toMat4(transform.Orientation);
		glm::mat4 view = glm::translate(rotate, -transform.Position);

		s_SceneData.ViewProjectionMatrix = camera.GetProjectionMatrix() * view;
		s_SceneData.CameraPosition = transform.Position;

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

	void Renderer::Submit(const Ref<Shader>& shader, Entity entity)
	{
		auto& transform = entity.GetComponent<TransformComponent>();
		auto& vao = entity.GetComponent<MeshComponent>().Mesh.GetVertexArray();

		shader->Attach();
		shader->SetMat4("u_ViewProjection", s_SceneData.ViewProjectionMatrix);
		shader->SetFloat3("u_CameraPosition", s_SceneData.CameraPosition);
		shader->SetMat4("u_Model", transform);

		vao->Attach();
		RenderCommand::Draw(vao);
	}

	void Renderer::Submit(const Ref<Shader>& shader, const Ref<VertexArray>& va, const glm::mat4& transform)
	{
		shader->Attach();
		shader->SetMat4("u_ViewProjection", s_SceneData.ViewProjectionMatrix);
		shader->SetFloat3("u_CameraPosition", s_SceneData.CameraPosition);
		shader->SetMat4("u_Model", transform);

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
