#include "qkpch.h"
#include "SceneRenderer.h"

#include "Quark/Core/Application.h"
#include "Cubemap.h"
#include "Renderer.h"
#include "RenderCommand.h"

namespace Quark {

	struct EnvironmentData
	{
		Ref<Shader> SkyboxShader;
		Ref<Shader> IrradianceShader;
		Ref<Shader> EquirectangleToCubemapShader;
		Ref<VertexArray> CubemapVAO;
		Ref<Framebuffer> Framebuffer;

		Ref<Cubemap> Environment;
		Ref<Cubemap> Irradiance;
	};

	struct SceneRendererData
	{
		Scope<EnvironmentData> Env;
	};

	static SceneRendererData s_Data;

	Vec2i SceneRenderer::s_ViewportSize{};
	Ref<Scene> SceneRenderer::s_ActiveScene = nullptr;

	void SceneRenderer::SetActiveScene(const Ref<Scene>& scene)
	{
		s_ActiveScene = scene;
		OnNewActiveScene();
	}

	void SceneRenderer::SetEnvironment(std::string_view filepath)
	{
		InitEnvironment(filepath);
	}

	void SceneRenderer::OnRender()
	{
		if (s_ActiveScene)
		{
			if (s_ActiveScene->m_CameraEntity)
			{
				auto& sceneCamera = s_ActiveScene->m_CameraEntity.GetComponent<CameraComponent>().Camera;
				auto& cameraTransform = s_ActiveScene->m_CameraEntity.GetComponent<Transform3DComponent>();

				Renderer::BeginScene(sceneCamera, cameraTransform);

				// Environment map
				if (s_Data.Env)
				{
					RenderCommand::SetCullFace(RenderCullFace::Front);
					RenderCommand::SetDepthFunction(RenderDepthFunction::LessEqual);

					Mat4f rotate = glm::toMat4(cameraTransform.Orientation);
					Mat4f view = glm::translate(rotate, (Vec3f)-cameraTransform.Position);

					s_Data.Env->SkyboxShader->Attach();
					s_Data.Env->SkyboxShader->SetMat4("u_View", view);
					s_Data.Env->SkyboxShader->SetMat4("u_Projection", sceneCamera.GetProjection());
					s_Data.Env->SkyboxShader->SetInt("u_EnvironmentMap", 0);
					s_Data.Env->SkyboxShader->SetFloat("u_Exposure", 1.0f);

					s_Data.Env->Environment->Attach(0);
					s_Data.Env->Irradiance->Attach(5); // TODO: put inside scene uniform buffer

					RenderCommand::DrawIndexed(s_Data.Env->CubemapVAO);
					RenderCommand::SetCullFace(RenderCullFace::Default);
					RenderCommand::SetDepthFunction(RenderDepthFunction::Default);
				}

				Renderer::EndScene();
			}
		}
	}

	void SceneRenderer::OnViewportResized(uint32_t width, uint32_t height)
	{
		s_ViewportSize = { width, height };

		if (s_ActiveScene)
		{
			auto view = s_ActiveScene->m_Registry.view<CameraComponent>();
			for (auto entity : view)
			{
				auto& cameraComponent = view.get<CameraComponent>(entity);
				cameraComponent.Camera.Resize(width, height);
			}
		}
	}

	void SceneRenderer::Dispose()
	{
		QK_PROFILE_FUNCTION();

		s_Data.Env.reset();
	}

	void SceneRenderer::OnNewActiveScene()
	{
		// Capture the original viewport dimensions when setting a new scene
		auto view = s_ActiveScene->m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			view.get<CameraComponent>(entity).Camera.Resize(s_ViewportSize.x, s_ViewportSize.y);
		}
	}

	void SceneRenderer::InitEnvironment(std::string_view environmentFilepath)
	{
		QK_PROFILE_FUNCTION();

		if (!s_Data.Env)
		{
			auto& assetDir = Application::Get().GetOptions().AssetDir;

			s_Data.Env = CreateScope<EnvironmentData>();
			s_Data.Env->SkyboxShader = Shader::Create(assetDir + "/assets/shaders/version/3.30/cubemap.glsl");
			s_Data.Env->IrradianceShader = Shader::Create(assetDir + "/assets/shaders/version/3.30/irradiance.glsl");
			s_Data.Env->EquirectangleToCubemapShader = Shader::Create(assetDir + "/assets/shaders/version/3.30/equirectangle_to_cubemap.glsl");

			{
				Mesh cube = Mesh::GenerateUnitCube();
				s_Data.Env->CubemapVAO = cube.GetVertexArray();
			}

			{
				CubemapSpecification spec;
				spec.Width = 2048;
				spec.Height = 2048;
				s_Data.Env->Environment = Cubemap::Create(spec);

				spec.Width = 32;
				spec.Height = 32;
				s_Data.Env->Irradiance = Cubemap::Create(spec);
			}

			{
				FramebufferSpecification spec;
				spec.Width = 2048;
				spec.Height = 2048;
				spec.Attachments = {  };
				s_Data.Env->Framebuffer = Framebuffer::Create(spec);
			}
		}

		glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
		glm::mat4 captureViews[] = {
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
		};

		auto hdrTexture = Texture2D::Create(environmentFilepath);
		hdrTexture->Attach(0);

		s_Data.Env->Framebuffer->Attach();

		s_Data.Env->EquirectangleToCubemapShader->Attach();
		s_Data.Env->EquirectangleToCubemapShader->SetInt("u_EquirectangularMap", 0);
		s_Data.Env->EquirectangleToCubemapShader->SetMat4("u_Projection", captureProjection);

		RenderCommand::SetCullFace(RenderCullFace::Front);
		RenderCommand::SetDepthFunction(RenderDepthFunction::LessEqual);

		for (uint8_t i = 0; i < 6; i++)
		{
			s_Data.Env->EquirectangleToCubemapShader->SetMat4("u_View", captureViews[i]);
			s_Data.Env->Framebuffer->AttachColorTextureTarget(0x8515 + i, s_Data.Env->Environment->GetRendererID());

			RenderCommand::Clear();
			RenderCommand::DrawIndexed(s_Data.Env->CubemapVAO);
		}

		s_Data.Env->Framebuffer->Detach();
		s_Data.Env->Framebuffer->Resize(32, 32);
		s_Data.Env->Framebuffer->Attach();

		s_Data.Env->IrradianceShader->Attach();
		s_Data.Env->IrradianceShader->SetInt("u_EnvironmentMap", 0);
		s_Data.Env->IrradianceShader->SetMat4("u_Projection", captureProjection);
		s_Data.Env->Environment->Attach(0);

		for (uint8_t i = 0; i < 6; i++)
		{
			s_Data.Env->IrradianceShader->SetMat4("u_View", captureViews[i]);
			s_Data.Env->Framebuffer->AttachColorTextureTarget(0x8515 + i, s_Data.Env->Irradiance->GetRendererID());

			RenderCommand::Clear();
			RenderCommand::DrawIndexed(s_Data.Env->CubemapVAO);
		}

		s_Data.Env->Framebuffer->Detach();

		RenderCommand::SetCullFace(RenderCullFace::Default);
		RenderCommand::SetDepthFunction(RenderDepthFunction::Default);

		// TODO: make a proper render pipeline that handles viewport resize
		auto& window = Application::Get().GetWindow();
		Renderer::OnViewportResized(window.GetWidth(), window.GetHeight());
	}
}
