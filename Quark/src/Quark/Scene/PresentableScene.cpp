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
					Renderer2D::DrawSprite(csrc.Color, transform.GetMatrix());
				}
			}

			// Textured sprites
			{
				auto view = m_Registry.view<TexturedSpriteRendererComponent, Transform3DComponent>();
				for (auto entity : view)
				{
					auto [tsrc, transform] = view.get<TexturedSpriteRendererComponent, Transform3DComponent>(entity);
					Renderer2D::DrawSprite(tsrc.Texture.get(), tsrc.Tint, transform.GetMatrix());
				}
			}

			// Text renderer components
			{
				auto view = m_Registry.view<TextRendererComponent, Transform3DComponent>();
				for (auto entity : view)
				{
					auto [trc, transform] = view.get<TextRendererComponent, Transform3DComponent>(entity);
					Renderer2D::DrawText(trc.Label, transform.GetMatrix());
				}
			}

			Renderer2D::EndScene();

			if (m_Data.Env)
			{
				Renderer::GetCommandBuffer()->SetCullMode(RenderCullMode::Back);
				Renderer::GetCommandBuffer()->SetDepthFunction(DepthCompareFunction::LessOrEqual);
				Renderer::BindPipeline(m_Data.Env->SkyboxPipeline.get());

				Mat4f cameraRotate = glm::toMat4(cameraTransform.Orientation);
				Mat4f cameraView = (Mat3f)cameraRotate;

				m_Data.Env->SkyboxShader->SetMat4f("u_View", cameraView);
				m_Data.Env->SkyboxShader->SetMat4f("u_Projection", sceneCamera.GetProjection());
				m_Data.Env->SkyboxShader->SetInt("u_EnvironmentMap", 0);
				m_Data.Env->SkyboxShader->SetFloat("u_Exposure", 1.0f);

				m_Data.Env->Environment->Attach(0);
				m_Data.Env->Irradiance->Attach(5); // TODO: put inside scene uniform buffer

				Renderer::Submit(m_Data.Env->CubemapBox.GetVertexBuffer(), m_Data.Env->CubemapBox.GetIndexBuffer());
				Renderer::GetCommandBuffer()->SetCullMode(RenderCullMode::Default);
				Renderer::GetCommandBuffer()->SetDepthFunction(DepthCompareFunction::Default);
			}
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
		//NewEnvironment(filepath);
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
		QK_PROFILE_FUNCTION();

		if (!m_Data.Env)
		{
			auto& coreDirectory = Application::Get()->GetOptions().CoreDir;

			m_Data.Env = CreateScope<EnvironmentData>();
			m_Data.Env->SkyboxShader = Shader::Create((coreDirectory / "Quark/assets/shaders/version/3.30/CubemapSkybox.glsl").string());
			m_Data.Env->IrradianceShader = Shader::Create((coreDirectory / "Quark/assets/shaders/version/3.30/Irradiance.glsl").string());
			m_Data.Env->EquirectangleToCubemapShader = Shader::Create((coreDirectory / "Quark/assets/shaders/version/3.30/EquirectangleToCubemap.glsl").string());

			m_Data.Env->CubemapBox = Mesh::GenerateUnitCube();

			{
				CubemapSpecification environmentSpec;
				environmentSpec.Width = 2048;
				environmentSpec.Height = 2048;
				m_Data.Env->Environment = Cubemap::Create(environmentSpec);

				CubemapSpecification irradianceSpec;
				irradianceSpec.Width = 32;
				irradianceSpec.Height = 32;
				m_Data.Env->Irradiance = Cubemap::Create(irradianceSpec);
			}

			{
				RenderPassSpecification spec;
				spec.BindPoint = PipelineBindPoint::Graphics;
				spec.ColorFormat = ColorFormat::RGBA8;
				spec.ClearBuffers = true;

				m_Data.Env->RenderPass = RenderPass::Create(spec);
			}

			{
				PipelineSpecification spec;
				spec.Layout = Mesh::GetBufferLayout();
				spec.Topology = PrimitiveTopology::TriangleList;
				spec.Shader = m_Data.Env->IrradianceShader.get();
				spec.RenderPass = m_Data.Env->RenderPass.get();

				m_Data.Env->IrradiancePipeline = Pipeline::Create(spec);
			}

			{
				PipelineSpecification spec;
				spec.Layout = Mesh::GetBufferLayout();
				spec.Topology = PrimitiveTopology::TriangleList;
				spec.Shader = m_Data.Env->EquirectangleToCubemapShader.get();
				spec.RenderPass = m_Data.Env->RenderPass.get();

				m_Data.Env->EnvironmentMapPipeline = Pipeline::Create(spec);
			}

			{
				PipelineSpecification spec;
				spec.Layout = Mesh::GetBufferLayout();
				spec.Topology = PrimitiveTopology::TriangleList;
				spec.Shader = m_Data.Env->SkyboxShader.get();
				spec.RenderPass = m_Data.Env->RenderPass.get();

				m_Data.Env->SkyboxPipeline = Pipeline::Create(spec);
			}

			{
				FramebufferAttachmentSpecification attachmentSpec = {
					2048,
					2048,
					Renderer::GetMultisampling(),
					ColorFormat::RGBA16f
				};

				m_Data.Env->ColorAttachment = FramebufferAttachment::Create(attachmentSpec);

				FramebufferSpecification spec;
				spec.Width = 2048;
				spec.Height = 2048;
				spec.RenderPass = m_Data.Env->RenderPass.get();
				spec.Attachments = { m_Data.Env->ColorAttachment.get() };

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
		Renderer::GetCommandBuffer()->BindTexture(hdrTexture.get(), 0);

		Renderer::GetCommandBuffer()->SetCullMode(RenderCullMode::Front);
		Renderer::GetCommandBuffer()->SetDepthFunction(DepthCompareFunction::LessOrEqual);

		{
			Renderer::BindPipeline(m_Data.Env->EnvironmentMapPipeline.get());
			Renderer::BeginRenderPass(m_Data.Env->RenderPass.get(), m_Data.Env->Framebuffer.get());

			m_Data.Env->EquirectangleToCubemapShader->SetInt("u_EquirectangularMap", 0);
			m_Data.Env->EquirectangleToCubemapShader->SetMat4f("u_Projection", captureProjection);

			for (uint8_t i = 0; i < 6; i++)
			{
				m_Data.Env->EquirectangleToCubemapShader->SetMat4f("u_View", captureViews[i]);
				//m_Data.Env->Framebuffer->AttachColorTextureTarget(0x8515 + i, m_Data.Env->Environment->GetRendererID());

				// FIX:
				//RenderCommand::Clear();
				Renderer::Submit(m_Data.Env->CubemapBox.GetVertexBuffer(), m_Data.Env->CubemapBox.GetIndexBuffer());
			}

			Renderer::EndRenderPass();
		}

		m_Data.Env->Framebuffer->Resize(32, 32);

		{
			Renderer::BindPipeline(m_Data.Env->IrradiancePipeline.get());
			Renderer::BeginRenderPass(m_Data.Env->RenderPass.get(), m_Data.Env->Framebuffer.get());

			m_Data.Env->IrradianceShader->SetInt("u_EnvironmentMap", 0);
			m_Data.Env->IrradianceShader->SetMat4f("u_Projection", captureProjection);
			m_Data.Env->Environment->Attach(0);

			for (uint8_t i = 0; i < 6; i++)
			{
				m_Data.Env->IrradianceShader->SetMat4f("u_View", captureViews[i]);
				//m_Data.Env->Framebuffer->AttachColorTextureTarget(0x8515 + i, m_Data.Env->Irradiance->GetRendererID());

				// FIX:
				//RenderCommand::Clear();
				Renderer::Submit(m_Data.Env->CubemapBox.GetVertexBuffer(), m_Data.Env->CubemapBox.GetIndexBuffer());
			}

			Renderer::EndRenderPass();
		}

		Renderer::GetCommandBuffer()->SetCullMode(RenderCullMode::Default);
		Renderer::GetCommandBuffer()->SetDepthFunction(DepthCompareFunction::Default);
	}
}
