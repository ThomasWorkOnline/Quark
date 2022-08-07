#include "qkpch.h"
#include "Renderer.h"

#include "CommandBuffer.h"
#include "GraphicsContext.h"
#include "RenderCommand.h"

namespace Quark {

	struct RendererData
	{
		uint32_t MaxUniformBuffers = 0;
		ShaderLibrary ShaderLib;

		CommandBuffer* ActiveCommandBuffer = nullptr;
	};

	static Scope<RendererData> s_Data;

	void Renderer::Initialize()
	{
		QK_PROFILE_FUNCTION();

		s_Data = CreateScope<RendererData>();
		s_Data->MaxUniformBuffers = GraphicsAPI::Instance->GetHardwareConstraints().UniformBufferConstraints.MaxBindings;
	}

	void Renderer::Dispose()
	{
		QK_PROFILE_FUNCTION();
		s_Data.reset();
	}

	void Renderer::BeginFrame()
	{
		GraphicsContext::Get()->StartFrame();

		s_Data->ActiveCommandBuffer = GetCommandBuffer().get();
		s_Data->ActiveCommandBuffer->Reset();
		s_Data->ActiveCommandBuffer->Begin();
	}

	void Renderer::EndFrame()
	{
		s_Data->ActiveCommandBuffer->End();
		GraphicsContext::Get()->Submit();
	}

	void Renderer::BindPipeline(const Ref<Pipeline>& pipeline)
	{
		s_Data->ActiveCommandBuffer->BindPipeline(pipeline);
	}

	void Renderer::BeginRenderPass(const Ref<RenderPass>& renderPass, const Ref<Framebuffer>& framebuffer)
	{
		s_Data->ActiveCommandBuffer->BeginRenderPass(renderPass, framebuffer);
	}

	void Renderer::EndRenderPass()
	{
		s_Data->ActiveCommandBuffer->EndRenderPass();
	}

	void Renderer::OnViewportResized(uint32_t width, uint32_t height)
	{
		GraphicsContext::Get()->OnViewportResized(width, height);
	}

	const Ref<CommandBuffer>& Renderer::GetCommandBuffer()
	{
		return GraphicsContext::Get()->GetCommandBuffer();
	}

	ShaderLibrary& Renderer::GetShaderLibrary()
	{
		return s_Data->ShaderLib;
	}
}
