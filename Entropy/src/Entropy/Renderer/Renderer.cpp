#include "Renderer.h"

#include "../Core/Core.h"
#include "../Scene/Scene.h"
#include "../Scene/Entity.h"
#include "../Scene/Components.h"

#include "RenderingAPI.h"

namespace Entropy {

	Renderer::SceneData Renderer::s_SceneData;

	void Renderer::Init()
	{
		RenderCommand::Init();
	}

	void Renderer::Dispose()
	{
	}

	void Renderer::BeginScene(Entity cameraEntity)
	{
		auto& transform = cameraEntity.GetComponent<TransformComponent>();
		Camera& camera = cameraEntity.GetComponent<CameraComponent>();

		transform.Orientation = glm::normalize(transform.Orientation);
		glm::mat4 rotate = glm::toMat4(transform.Orientation);
		glm::mat4 view = glm::translate(rotate, -transform.Position);

		s_SceneData.ProjectionMatrix = camera.GetProjectionMatrix();
		s_SceneData.ViewMatrix = view;
	}

	void Renderer::BeginScene(const glm::mat4& cameraProjection, const glm::mat4& cameraView)
	{
		s_SceneData.ProjectionMatrix = cameraProjection;
		s_SceneData.ViewMatrix = cameraView;
	}

	void Renderer::EndScene()
	{

	}

	void Renderer::Submit(const Ref<Shader>& shader, Entity entity)
	{
		auto& transform = entity.GetComponent<TransformComponent>();
		auto& vao = entity.GetComponent<MeshComponent>().Mesh.GetVertexArray();

		shader->Attach();
		shader->SetMat4("u_Projection", s_SceneData.ProjectionMatrix);
		shader->SetMat4("u_View", s_SceneData.ViewMatrix);
		shader->SetMat4("u_Model", transform);

		vao->Attach();
		RenderCommand::Draw(vao);
	}

	void Renderer::Submit(const Ref<Shader>& shader, const Ref<VertexArray>& va, const glm::mat4& transform)
	{
		shader->Attach();
		shader->SetMat4("u_Projection", s_SceneData.ProjectionMatrix);
		shader->SetMat4("u_View", s_SceneData.ViewMatrix);
		shader->SetMat4("u_Model", transform);

		va->Attach();
		RenderCommand::Draw(va);
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		RenderCommand::SetViewport(0, 0, width, height);
	}
}
