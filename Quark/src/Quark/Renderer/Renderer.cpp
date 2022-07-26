#include "qkpch.h"
#include "Renderer.h"

#include "CommandBuffer.h"
#include "RenderCommand.h"

namespace Quark {

	struct RendererData
	{
		uint32_t MaxUniformBuffers = 0;

		// Ensure std140 layout
		struct CameraData
		{
			Mat4f View;
			Mat4f Projection;
		};

		CameraData         CameraBufferData{};
		Ref<Texture2D>     DefaultTexture;
		ShaderLibrary      ShaderLib;
	};

	static RendererData* s_Data;

	void Renderer::Initialize()
	{
		QK_PROFILE_FUNCTION();

		RenderCommand::Init();

		QK_CORE_ASSERT(s_Data == nullptr, "s_Data already initialized");
		s_Data = new RendererData();
		s_Data->MaxUniformBuffers = GraphicsAPI::Instance->GetHardwareConstraints().UniformBufferConstraints.MaxBindings;

		uint32_t textureColor = 0xffffffff;
		Texture2DSpecification spec = { 1, 1, 1,
			ColorDataFormat::RGBA,
			InternalColorFormat::RGBA8,
			TextureFilteringMode::Nearest, TextureFilteringMode::Nearest, TextureTilingMode::Repeat
		};

		s_Data->DefaultTexture = Texture2D::Create(spec);
		s_Data->DefaultTexture->SetData(&textureColor, sizeof(uint32_t));
	}

	void Renderer::Dispose()
	{
		QK_PROFILE_FUNCTION();

		delete s_Data;
		s_Data = nullptr;
	}

	void Renderer::BeginScene(const Camera& camera, const Transform3DComponent& cameraTransform)
	{
		Mat4f rotate = glm::toMat4(cameraTransform.Orientation);
		Mat4f view = glm::translate(rotate, (Vec3f)-cameraTransform.Position);

		BeginScene(camera.GetProjection(), view);
	}

	void Renderer::BeginScene(const Mat4f& cameraProjection, const Mat4f& cameraView)
	{
	}

	void Renderer::EndScene()
	{
	}

	void Renderer::BeginFrame()
	{
	}

	void Renderer::EndFrame()
	{
	}

	ShaderLibrary& Renderer::GetShaderLibrary()
	{
		return s_Data->ShaderLib;
	}
}
