#include "qkpch.h"
#include "Renderer.h"

#include "CommandBuffer.h"
#include "GraphicsContext.h"
#include "RenderCommand.h"

namespace Quark {

	struct RendererData
	{
		uint32_t MaxUniformBuffers = 0;
		uint32_t FramesInFlight = 0;
		ShaderLibrary ShaderLib;

		CommandBuffer* ActiveCommandBuffer = nullptr;
		std::vector<Ref<CommandBuffer>> CommandBuffers;

		uint32_t CurrentFrameIndex = std::numeric_limits<uint32_t>::max();
	};

	static Scope<RendererData> s_Data;

	void Renderer::Initialize()
	{
		QK_PROFILE_FUNCTION();

		s_Data = CreateScope<RendererData>();
		s_Data->MaxUniformBuffers = GraphicsAPI::Instance->GetHardwareConstraints().UniformBufferConstraints.MaxBindings;
		s_Data->FramesInFlight = GraphicsAPI::Instance->GetRendererFramesInFlight();

		s_Data->CommandBuffers.resize(s_Data->FramesInFlight);
		for (uint32_t i = 0; i < s_Data->FramesInFlight; i++)
		{
			s_Data->CommandBuffers[i] = CommandBuffer::Create();
		}
	}

	void Renderer::Dispose()
	{
		QK_PROFILE_FUNCTION();
		s_Data.reset();
	}

	void Renderer::BeginFrame()
	{
		s_Data->CurrentFrameIndex = (s_Data->CurrentFrameIndex + 1) % s_Data->FramesInFlight;
		s_Data->ActiveCommandBuffer = s_Data->CommandBuffers[s_Data->CurrentFrameIndex].get();

		RenderCommand::WaitForFences();

		GraphicsContext::Get().GetSwapChain()->AcquireNextImage();

		s_Data->ActiveCommandBuffer->Reset();
		s_Data->ActiveCommandBuffer->Begin();
	}

	void Renderer::EndFrame()
	{
		s_Data->ActiveCommandBuffer->End();
		RenderCommand::Submit();
	}

	void Renderer::BeginRenderPass(const Ref<RenderPass>& renderPass, const Ref<Framebuffer>& framebuffer)
	{
		s_Data->ActiveCommandBuffer->BeginRenderPass(renderPass, framebuffer);
	}

	void Renderer::EndRenderPass()
	{
		s_Data->ActiveCommandBuffer->EndRenderPass();
	}

	const Ref<CommandBuffer>& Renderer::GetCommandBuffer()
	{
		return s_Data->CommandBuffers[s_Data->CurrentFrameIndex];
	}

	void Renderer::OnViewportResized(uint32_t width, uint32_t height)
	{
		GraphicsContext::Get().GetSwapChain()->Resize(width, height);
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
