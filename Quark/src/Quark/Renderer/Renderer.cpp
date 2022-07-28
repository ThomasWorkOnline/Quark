#include "qkpch.h"
#include "Renderer.h"

#include "CommandBuffer.h"
#include "RenderCommand.h"

namespace Quark {

	struct RendererData
	{
		uint32_t MaxUniformBuffers = 0;
		uint32_t FramesInFlight = 2;

		Ref<Texture2D> DefaultTexture;
		ShaderLibrary  ShaderLib;

		uint32_t CurrentFrameIndex = std::numeric_limits<uint32_t>::max();
	};

	static Scope<RendererData> s_Data;

	void Renderer::Initialize()
	{
		QK_PROFILE_FUNCTION();

		s_Data = CreateScope<RendererData>();
		s_Data->MaxUniformBuffers = GraphicsAPI::Instance->GetHardwareConstraints().UniformBufferConstraints.MaxBindings;

		if (GraphicsAPI::GetAPI() == API::OpenGL)
		{
			s_Data->FramesInFlight = 1;
		}

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
		s_Data.reset();
	}

	void Renderer::BeginFrame()
	{
		s_Data->CurrentFrameIndex = (s_Data->CurrentFrameIndex + 1) % s_Data->FramesInFlight;
	}

	void Renderer::EndFrame()
	{
	}

	uint32_t Renderer::GetFramesInFlight()
	{
		return s_Data->FramesInFlight;
	}

	uint32_t Renderer::GetCurrentFrameIndex()
	{
		return s_Data->CurrentFrameIndex;
	}

	ShaderLibrary& Renderer::GetShaderLibrary()
	{
		return s_Data->ShaderLib;
	}
}
