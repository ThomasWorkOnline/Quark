#include "qkpch.h"
#include "PresentableScene.h"

#include "Quark/Core/Application.h"
#include "Quark/Renderer/Renderer.h"
#include "Quark/Renderer/Renderer2D.h"

#include "Components.h"

namespace Quark {

	void PresentableScene::OnRender()
	{
		if (m_PrimaryCameraEntity)
		{
			auto& sceneCamera = m_PrimaryCameraEntity.GetComponent<CameraComponent>().Camera;
			auto& cameraTransform = m_PrimaryCameraEntity.GetComponent<Transform3DComponent>();

			Renderer2D::BeginScene(sceneCamera, cameraTransform);

			// Colored sprites
			{
				auto view = m_Registry.view<SpriteRendererComponent, Transform3DComponent>();
				for (auto entity : view)
				{
					auto [csrc, transform] = view.get<SpriteRendererComponent, Transform3DComponent>(entity);
					Renderer2D::DrawSprite(csrc.Color, transform);
				}
			}

			// Textured sprites
			{
				auto view = m_Registry.view<TexturedSpriteRendererComponent, Transform3DComponent>();
				for (auto entity : view)
				{
					auto [tsrc, transform] = view.get<TexturedSpriteRendererComponent, Transform3DComponent>(entity);
					Renderer2D::DrawSprite(tsrc.Texture.get(), tsrc.Tint, transform);
				}
			}

			// Text renderer components
			{
				auto view = m_Registry.view<TextRendererComponent, Transform3DComponent>();
				for (auto entity : view)
				{
					auto [trc, transform] = view.get<TextRendererComponent, Transform3DComponent>(entity);
					Renderer2D::DrawText(trc.Label, transform);
				}
			}

			Renderer2D::EndScene();

#if 0
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
#endif
		}
	}

	void PresentableScene::OnEvent(Event& e)
	{
		if (!this) return; // In the case we call an event on an empty scene

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowResizedEvent>(ATTACH_EVENT_FN(OnWindowResized));

		Scene::OnEvent(e);
	}

	Entity PresentableScene::CreatePrimaryCamera()
	{
		auto entity = CreateEntity();
		entity.AddComponent<Transform3DComponent>();
		entity.AddComponent<PhysicsComponent>();
		entity.AddComponent<CameraComponent>().Camera.SetPerspective(90.0f);
		return m_PrimaryCameraEntity = entity;
	}

	void PresentableScene::SetPrimaryCamera(Entity cameraEntity)
	{
		QK_CORE_ASSERT(cameraEntity.HasComponent<CameraComponent>(), "Entity must have a camera component");
		m_PrimaryCameraEntity = cameraEntity;
	}

	void PresentableScene::SetEnvironment(std::string_view filepath)
	{
		NewEnvironment(filepath);
	}

	void PresentableScene::OnWindowResized(WindowResizedEvent& e)
	{
		if (m_PrimaryCameraEntity)
		{
			auto& cc = m_PrimaryCameraEntity.GetComponent<CameraComponent>();
			cc.Camera.Resize(e.GetWidth(), e.GetHeight());
		}
	}

	void PresentableScene::NewEnvironment(std::string_view filepath)
	{
#if 0
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
#endif
	}
}
