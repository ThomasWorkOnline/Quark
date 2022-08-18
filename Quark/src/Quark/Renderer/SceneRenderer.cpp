#include "qkpch.h"
#include "SceneRenderer.h"

#include "Renderer.h"
#include "Renderer2D.h"
#include "RenderCommand.h"

#include "Quark/Core/Application.h"
#include "Quark/Scene/Components.h"

namespace Quark {

	void SceneRenderer::SetContext(Ref<Scene> scene)
	{
		QK_CORE_ASSERT(scene, "Scene is nullptr!");

		m_Scene = std::move(scene);

		if (m_Scene->HasPrimaryCamera())
		{
			auto& viewport = Renderer::GetViewportExtent();

			auto& cc = m_Scene->m_PrimaryCameraEntity.GetComponent<CameraComponent>();
			cc.Camera.Resize(viewport.Width, viewport.Height);
		}
	}

	void SceneRenderer::OnRender()
	{
		if (m_Scene && m_Scene->HasPrimaryCamera())
		{
			auto& sceneCamera = m_Scene->m_PrimaryCameraEntity.GetComponent<CameraComponent>().Camera;
			auto& cameraTransform = m_Scene->m_PrimaryCameraEntity.GetComponent<Transform3DComponent>();

			Renderer2D::BeginScene(sceneCamera, cameraTransform);

			// Colored sprites
			{
				auto view = m_Scene->m_Registry.view<ColoredSpriteRendererComponent, Transform3DComponent>();
				for (auto entity : view)
				{
					auto [csr, transform] = view.get<ColoredSpriteRendererComponent, Transform3DComponent>(entity);
					Renderer2D::DrawSprite(csr.Color, transform);
				}
			}

			// Textured sprites
			{
				auto view = m_Scene->m_Registry.view<TexturedSpriteRendererComponent, Transform3DComponent>();
				for (auto entity : view)
				{
					auto [tsr, transform] = view.get<TexturedSpriteRendererComponent, Transform3DComponent>(entity);
					Renderer2D::DrawSprite(tsr.Texture.get(), tsr.Tint, transform);
				}
			}

			Renderer2D::EndScene();

			if (m_Data.Env)
			{
				RenderCommand::SetCullFace(RenderCullMode::Front);
				RenderCommand::SetDepthFunction(RenderDepthFunction::LessEqual);

				Mat4f cameraRotate = glm::toMat4(cameraTransform.Orientation);
				Mat4f cameraView = glm::translate(cameraRotate, (Vec3f)-cameraTransform.Position);

				m_Data.Env->SkyboxShader->SetMat4f("u_View", cameraView);
				m_Data.Env->SkyboxShader->SetMat4f("u_Projection", sceneCamera.GetProjection());
				m_Data.Env->SkyboxShader->SetInt("u_EnvironmentMap", 0);
				m_Data.Env->SkyboxShader->SetFloat("u_Exposure", 1.0f);

				m_Data.Env->Environment->Attach(0);
				m_Data.Env->Irradiance->Attach(5); // TODO: put inside scene uniform buffer

				Renderer::Submit(m_Data.Env->CubemapVertexBuffer.get(), m_Data.Env->CubemapIndexBuffer.get());
				RenderCommand::SetCullFace(RenderCullMode::Default);
				RenderCommand::SetDepthFunction(RenderDepthFunction::Default);
			}
		}
	}

	void SceneRenderer::OnViewportResized(uint32_t viewportWidth, uint32_t viewportHeight)
	{
		if (m_Scene && m_Scene->HasPrimaryCamera())
		{
			auto& cc = m_Scene->m_PrimaryCameraEntity.GetComponent<CameraComponent>();
			cc.Camera.Resize(viewportWidth, viewportHeight);
		}
	}

	void SceneRenderer::SetEnvironment(std::string_view filepath)
	{
		NewEnvironment(filepath);
	}

	void SceneRenderer::NewEnvironment(std::string_view filepath)
	{
		QK_PROFILE_FUNCTION();

		if (!m_Data.Env)
		{
			auto& assetDir = Application::Get()->GetOptions().AssetDir;

			m_Data.Env = CreateScope<EnvironmentData>();
			m_Data.Env->SkyboxShader = Shader::Create((assetDir / "assets/shaders/version/3.30/cubemap.glsl").string());
			m_Data.Env->IrradianceShader = Shader::Create((assetDir / "assets/shaders/version/3.30/irradiance.glsl").string());
			m_Data.Env->EquirectangleToCubemapShader = Shader::Create((assetDir / "assets/shaders/version/3.30/equirectangle_to_cubemap.glsl").string());

			{
				Mesh cube = Mesh::GenerateUnitCube();
				m_Data.Env->CubemapVertexBuffer.reset(cube.GetVertexBuffer());
				m_Data.Env->CubemapIndexBuffer.reset(cube.GetIndexBuffer());
			}

			{
				CubemapSpecification spec;
				spec.Width = 2048;
				spec.Height = 2048;
				m_Data.Env->Environment = Cubemap::Create(spec);

				spec.Width = 32;
				spec.Height = 32;
				m_Data.Env->Irradiance = Cubemap::Create(spec);
			}

			{
				FramebufferAttachmentSpecification attachmentSpec = {
					ColorDataFormat::RGBA16f
				};

				FramebufferSpecification spec;
				spec.Width = 2048;
				spec.Height = 2048;
				spec.Attachments = { Ref<FramebufferAttachment>(FramebufferAttachment::Create(nullptr, attachmentSpec)) };
				m_Data.Env->Framebuffer = Framebuffer::Create(spec);
			}
		}

		Mat4f captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
		Mat4f captureViews[] = {
			glm::lookAt(Vec3f(0.0f, 0.0f, 0.0f), Vec3f( 1.0f,  0.0f,  0.0f), Vec3f(0.0f, -1.0f,  0.0f)),
			glm::lookAt(Vec3f(0.0f, 0.0f, 0.0f), Vec3f(-1.0f,  0.0f,  0.0f), Vec3f(0.0f, -1.0f,  0.0f)),
			glm::lookAt(Vec3f(0.0f, 0.0f, 0.0f), Vec3f( 0.0f,  1.0f,  0.0f), Vec3f(0.0f,  0.0f, -1.0f)),
			glm::lookAt(Vec3f(0.0f, 0.0f, 0.0f), Vec3f( 0.0f, -1.0f,  0.0f), Vec3f(0.0f,  0.0f,  1.0f)),
			glm::lookAt(Vec3f(0.0f, 0.0f, 0.0f), Vec3f( 0.0f,  0.0f, -1.0f), Vec3f(0.0f, -1.0f,  0.0f)),
			glm::lookAt(Vec3f(0.0f, 0.0f, 0.0f), Vec3f( 0.0f,  0.0f,  1.0f), Vec3f(0.0f, -1.0f,  0.0f))
		};

		auto hdrTexture = Texture2D::Create(filepath);
		hdrTexture->Attach(0);

		m_Data.Env->Framebuffer->Attach();
		m_Data.Env->EquirectangleToCubemapShader->SetInt("u_EquirectangularMap", 0);
		m_Data.Env->EquirectangleToCubemapShader->SetMat4f("u_Projection", captureProjection);

		RenderCommand::SetCullFace(RenderCullMode::Front);
		RenderCommand::SetDepthFunction(RenderDepthFunction::LessEqual);

		for (uint8_t i = 0; i < 6; i++)
		{
			m_Data.Env->EquirectangleToCubemapShader->SetMat4f("u_View", captureViews[i]);
			//m_Data.Env->Framebuffer->AttachColorTextureTarget(0x8515 + i, m_Data.Env->Environment->GetRendererID());

			// FIX:
			//RenderCommand::Clear();
			Renderer::Submit(m_Data.Env->CubemapVertexBuffer.get(), m_Data.Env->CubemapIndexBuffer.get());
		}

		m_Data.Env->Framebuffer->Detach();
		m_Data.Env->Framebuffer->Resize(32, 32);
		m_Data.Env->Framebuffer->Attach();

		m_Data.Env->IrradianceShader->SetInt("u_EnvironmentMap", 0);
		m_Data.Env->IrradianceShader->SetMat4f("u_Projection", captureProjection);
		m_Data.Env->Environment->Attach(0);

		for (uint8_t i = 0; i < 6; i++)
		{
			m_Data.Env->IrradianceShader->SetMat4f("u_View", captureViews[i]);
			//m_Data.Env->Framebuffer->AttachColorTextureTarget(0x8515 + i, m_Data.Env->Irradiance->GetRendererID());

			// FIX:
			//RenderCommand::Clear();
			Renderer::Submit(m_Data.Env->CubemapVertexBuffer.get(), m_Data.Env->CubemapIndexBuffer.get());
		}

		m_Data.Env->Framebuffer->Detach();

		RenderCommand::SetCullFace(RenderCullMode::Default);
		RenderCommand::SetDepthFunction(RenderDepthFunction::Default);
	}
}
