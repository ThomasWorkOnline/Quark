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

	void Renderer::BindPipeline(const Ref<Pipeline>& pipeline)
	{
		QK_ASSERT_RENDER_THREAD();
		s_Data->ActiveCommandBuffer->BindPipeline(pipeline);
	}

	void Renderer::BeginRenderPass(const Ref<RenderPass>& renderPass, const Ref<Framebuffer>& framebuffer)
	{
		QK_ASSERT_RENDER_THREAD();
		s_Data->ActiveCommandBuffer->BeginRenderPass(renderPass, framebuffer);
	}

	void Renderer::EndRenderPass()
	{
		QK_ASSERT_RENDER_THREAD();
		s_Data->ActiveCommandBuffer->EndRenderPass();
	}

	void Renderer::Submit(const Ref<VertexBuffer>& vertexBuffer, const Ref<IndexBuffer>& indexBuffer)
	{
		QK_ASSERT_RENDER_THREAD();

		s_Data->ActiveCommandBuffer->BindVertexBuffer(vertexBuffer, 0);
		s_Data->ActiveCommandBuffer->BindIndexBuffer(indexBuffer);
		s_Data->ActiveCommandBuffer->DrawIndexed(indexBuffer->GetCount());
	}

	void Renderer::Submit(const Ref<VertexBuffer>& vertexBuffer, const Ref<IndexBuffer>& indexBuffer, uint32_t indexCount)
	{
		QK_ASSERT_RENDER_THREAD();

		s_Data->ActiveCommandBuffer->BindVertexBuffer(vertexBuffer, 0);
		s_Data->ActiveCommandBuffer->BindIndexBuffer(indexBuffer);
		s_Data->ActiveCommandBuffer->DrawIndexed(indexCount);
	}

	void Renderer::DrawLines(const Ref<VertexBuffer>& vertexBuffer, uint32_t vertexCount)
	{
		QK_ASSERT_RENDER_THREAD();

		s_Data->ActiveCommandBuffer->BindVertexBuffer(vertexBuffer, 0);
		s_Data->ActiveCommandBuffer->DrawLines(vertexCount);
	}

	const Ref<CommandBuffer>& Renderer::GetCommandBuffer()
	{
		QK_ASSERT_RENDER_THREAD();
		return GraphicsContext::Get()->GetCommandBuffer();
	}

	ShaderLibrary& Renderer::GetShaderLibrary()
	{
		return s_Data->ShaderLib;
	}

	void Renderer::BeginFrame()
	{
		QK_ASSERT_RENDER_THREAD();

		GraphicsContext::Get()->StartFrame();

		s_Data->ActiveCommandBuffer = GetCommandBuffer().get();
		s_Data->ActiveCommandBuffer->Reset();
		s_Data->ActiveCommandBuffer->Begin();
	}

	void Renderer::EndFrame()
	{
		QK_ASSERT_RENDER_THREAD();

		s_Data->ActiveCommandBuffer->End();
		GraphicsContext::Get()->Submit();
	}

	void Renderer::OnViewportResized(uint32_t width, uint32_t height)
	{
		QK_ASSERT_RENDER_THREAD();
		GraphicsContext::Get()->OnViewportResized(width, height);
	}

	void Renderer::Initialize()
	{
		QK_PROFILE_FUNCTION();

		s_ThreadId = std::this_thread::get_id();
		RenderCommand::Init();

		s_Data = CreateScope<RendererData>();
		s_Data->MaxUniformBuffers = GraphicsAPI::Instance->GetCapabilities().UniformBufferConstraints.MaxBindings;
	}

	void Renderer::Dispose()
	{
		QK_PROFILE_FUNCTION();
		s_Data.reset();
	}
}
