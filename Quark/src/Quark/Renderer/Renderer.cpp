#include "qkpch.h"
#include "Renderer.h"

#include "UniformBuffer.h"
#include "RenderCommand.h"

namespace Quark {

	struct RendererData
	{
		uint32_t MaxUniformBuffers = 0;
		Ref<Texture2D> DefaultTexture;

		// Ensure std140 layout
		struct CameraData
		{
			Mat4f ViewProjection;
		};

		CameraData CameraBufferData{};
		Ref<UniformBuffer> CameraUniformBuffer;
		ShaderLibrary ShaderLib;
	};

	static Scope<RendererData> s_Data;

	void Renderer::Initialize()
	{
		QK_PROFILE_FUNCTION();

		s_Data = CreateScope<RendererData>();

		uint32_t textureColor = 0xffffffff;
		Texture2DSpecification spec = { 1, 1, 1,
			TextureDataFormat::RGBA,
			TextureInternalFormat::RGBA8,
			TextureFilteringMode::Nearest, TextureFilteringMode::Nearest, TextureTilingMode::Repeat
		};

		s_Data->DefaultTexture = Texture2D::Create(spec);
		s_Data->DefaultTexture->SetData(&textureColor, sizeof(uint32_t));

		s_Data->MaxUniformBuffers = GraphicsAPI::Instance->GetHardwareConstraints().UniformBufferConstraints.MaxBindings;
		s_Data->CameraUniformBuffer = UniformBuffer::Create(sizeof(RendererData::CameraData), 0);
	}

	void Renderer::Dispose()
	{
		QK_PROFILE_FUNCTION();
		s_Data.reset();
	}

	void Renderer::BeginScene(const Camera& camera, const Transform3DComponent& cameraTransform)
	{
		Mat4f rotate = glm::toMat4(cameraTransform.Orientation);
		Mat4f view = glm::translate(rotate, (Vec3f)-cameraTransform.Position);

		BeginScene(camera.GetProjection(), view);
	}

	void Renderer::BeginScene(const Mat4f& cameraProjection, const Mat4f& cameraView)
	{
		QK_ASSERT_RENDER_THREAD();

		s_Data->CameraBufferData.ViewProjection = cameraProjection * cameraView;
		s_Data->CameraUniformBuffer->Attach();
		s_Data->CameraUniformBuffer->SetData(&s_Data->CameraBufferData, sizeof(RendererData::CameraData));
	}

	void Renderer::EndScene()
	{
	}

	void Renderer::Submit(const Ref<Shader>& shader, const Ref<VertexArray>& va, const Mat4f& transform)
	{
		QK_ASSERT_RENDER_THREAD();

		s_Data->DefaultTexture->Attach();

		shader->Attach();
		shader->SetMat4("u_Model", transform);

		RenderCommand::DrawIndexed(va);
	}

	void Renderer::Submit(const Ref<Shader>& shader, const Ref<Texture2D>& texture, const Ref<VertexArray>& va, const Mat4f& transform)
	{
		QK_ASSERT_RENDER_THREAD();

		texture->Attach();

		shader->Attach();
		shader->SetMat4("u_Model", transform);

		RenderCommand::DrawIndexed(va);
	}

	void Renderer::Submit(const Ref<Shader>& shader, const Ref<Framebuffer>& framebuffer, const Ref<VertexArray>& va, const Mat4f& transform)
	{
		QK_ASSERT_RENDER_THREAD();

		framebuffer->AttachColorAttachment(0);

		shader->Attach();
		shader->SetMat4("u_Model", transform);

		RenderCommand::DrawIndexed(va);
	}

	void Renderer::OnViewportResized(uint32_t width, uint32_t height)
	{
		QK_ASSERT_RENDER_THREAD();
		RenderCommand::SetViewport(0, 0, width, height);
	}

	ShaderLibrary& Renderer::GetShaderLibrary()
	{
		return s_Data->ShaderLib;
	}
}
