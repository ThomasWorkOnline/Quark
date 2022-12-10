#include "qkpch.h"
#include "SceneRenderer.h"

#include "Quark/Core/Application.h"
#include "Renderer2D.h"

namespace Quark {

	SceneRenderer::SceneRenderer(Scene* scene)
		: m_Scene(scene)
	{
	}

	void SceneRenderer::OnRender()
	{
		if (m_PrimaryCameraEntity)
		{
			auto& sceneCamera = m_PrimaryCameraEntity.GetComponent<CameraComponent>().Camera;
			auto& cameraTransform = m_PrimaryCameraEntity.GetComponent<Transform3DComponent>();

			Renderer2D::BeginScene(sceneCamera, cameraTransform);

			// Colored sprites
			{
				auto view = m_Scene->GetRegistry().view<SpriteRendererComponent, Transform3DComponent>();
				for (auto entity : view)
				{
					auto [csrc, transform] = view.get<SpriteRendererComponent, Transform3DComponent>(entity);
					Renderer2D::DrawQuad(csrc.Color, transform.GetMatrix());
				}
			}

			// Textured sprites
			{
				auto view = m_Scene->GetRegistry().view<TexturedSpriteRendererComponent, Transform3DComponent>();
				for (auto entity : view)
				{
					auto [tsrc, transform] = view.get<TexturedSpriteRendererComponent, Transform3DComponent>(entity);
					Renderer2D::DrawSprite(tsrc.Texture.get(), tsrc.Tint, transform.GetMatrix());
				}
			}

			// Text renderer components
			{
				auto view = m_Scene->GetRegistry().view<TextRendererComponent, Transform3DComponent>();
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

				Renderer::BindTexture(m_Data.Env->Environment.get(), nullptr, 0);
				Renderer::BindTexture(m_Data.Env->Environment.get(), nullptr, 5);

				Renderer::DrawIndexed(m_Data.Env->CubemapBox.VertexBuffer.get(), m_Data.Env->CubemapBox.IndexBuffer.get());
				Renderer::GetCommandBuffer()->SetCullMode(RenderCullMode::Default);
				Renderer::GetCommandBuffer()->SetDepthFunction(DepthCompareFunction::Default);
			}
		}
	}

	void SceneRenderer::OnViewportResized(uint32_t viewportWidth, uint32_t viewportHeight)
	{
		if (m_PrimaryCameraEntity)
		{
			auto& cc = m_PrimaryCameraEntity.GetComponent<CameraComponent>();
			cc.Camera.Resize((float)viewportWidth, (float)viewportHeight);
		}
	}

	void SceneRenderer::SetPrimaryCamera(Entity cameraEntity)
	{
		QK_CORE_ASSERT(cameraEntity.HasComponent<CameraComponent>(), "Entity must have a camera component");
		m_PrimaryCameraEntity = cameraEntity;
	}

	void SceneRenderer::SetEnvironment(std::string_view filepath)
	{
		//NewEnvironment(filepath);
	}

	void SceneRenderer::NewEnvironment(std::string_view filepath)
	{
		QK_PROFILE_FUNCTION();

		if (!m_Data.Env)
		{
			auto& coreDirectory = Application::Get()->GetSpecification().CoreDir;

			m_Data.Env = CreateScope<EnvironmentData>();
			m_Data.Env->SkyboxShader = Shader::Create((coreDirectory / "assets/shaders/version/3.30/CubemapSkybox.glsl").string());
			m_Data.Env->IrradianceShader = Shader::Create((coreDirectory / "assets/shaders/version/3.30/Irradiance.glsl").string());
			m_Data.Env->EquirectangleToCubemapShader = Shader::Create((coreDirectory / "assets/shaders/version/3.30/EquirectangleToCubemap.glsl").string());

			m_Data.Env->CubemapBox = StaticMesh::GenerateUnitCube();

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
				spec.ColorAttachmentFormat = ColorFormat::RGBA8;
				spec.DepthAttachmentFormat = ColorFormat::Depth32f;
				spec.ClearBuffers = true;

				m_Data.Env->RenderPass = RenderPass::Create(spec);
			}

			{
				PipelineSpecification spec;
				spec.Layout = StaticMesh::GetBufferLayout();
				spec.Topology = PrimitiveTopology::TriangleList;
				spec.Shader = m_Data.Env->IrradianceShader.get();
				spec.RenderPass = m_Data.Env->RenderPass.get();

				m_Data.Env->IrradiancePipeline = Pipeline::Create(spec);
			}

			{
				PipelineSpecification spec;
				spec.Layout = StaticMesh::GetBufferLayout();
				spec.Topology = PrimitiveTopology::TriangleList;
				spec.Shader = m_Data.Env->EquirectangleToCubemapShader.get();
				spec.RenderPass = m_Data.Env->RenderPass.get();

				m_Data.Env->EnvironmentMapPipeline = Pipeline::Create(spec);
			}

			{
				PipelineSpecification spec;
				spec.Layout = StaticMesh::GetBufferLayout();
				spec.Topology = PrimitiveTopology::TriangleList;
				spec.Shader = m_Data.Env->SkyboxShader.get();
				spec.RenderPass = m_Data.Env->RenderPass.get();

				m_Data.Env->SkyboxPipeline = Pipeline::Create(spec);
			}

			{
				FramebufferAttachmentSpecification attachmentSpec = {
					2048,
					2048,
					Renderer::GetSupportedMSAA(),
					ColorFormat::RGBA16f
				};

				FramebufferSpecification spec;
				spec.Width = 2048;
				spec.Height = 2048;
				spec.RenderPass = m_Data.Env->RenderPass.get();
				spec.Attachments = { FramebufferAttachment::Create(attachmentSpec) };

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

		Renderer::GetCommandBuffer()->BindTexture(hdrTexture.get(), nullptr, Renderer::GetCurrentFrameIndex(), 0);
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
				Renderer::DrawIndexed(m_Data.Env->CubemapBox.VertexBuffer.get(), m_Data.Env->CubemapBox.IndexBuffer.get());
			}

			Renderer::EndRenderPass();
		}

		m_Data.Env->Framebuffer->Resize(32, 32);

		{
			Renderer::BindPipeline(m_Data.Env->IrradiancePipeline.get());
			Renderer::BeginRenderPass(m_Data.Env->RenderPass.get(), m_Data.Env->Framebuffer.get());

			m_Data.Env->IrradianceShader->SetInt("u_EnvironmentMap", 0);
			m_Data.Env->IrradianceShader->SetMat4f("u_Projection", captureProjection);

			Renderer::GetCommandBuffer()->BindTexture(m_Data.Env->Environment.get(), nullptr, Renderer::GetCurrentFrameIndex(), 0);

			for (uint8_t i = 0; i < 6; i++)
			{
				m_Data.Env->IrradianceShader->SetMat4f("u_View", captureViews[i]);
				//m_Data.Env->Framebuffer->AttachColorTextureTarget(0x8515 + i, m_Data.Env->Irradiance->GetRendererID());

				// FIX:
				//RenderCommand::Clear();
				Renderer::DrawIndexed(m_Data.Env->CubemapBox.VertexBuffer.get(), m_Data.Env->CubemapBox.IndexBuffer.get());
			}

			Renderer::EndRenderPass();
		}

		Renderer::GetCommandBuffer()->SetCullMode(RenderCullMode::Default);
		Renderer::GetCommandBuffer()->SetDepthFunction(DepthCompareFunction::Default);
	}
}
