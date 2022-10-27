#include "qkpch.h"
#include "Renderer.h"

#include "Quark/Filesystem/Filesystem.h"

#include "GraphicsContext.h"
#include "GraphicsAPI.cpp"

namespace Quark {

	struct RendererData
	{
		static constexpr uint32_t FramesInFlight = 2;

		ShaderLibrary ShaderLib;
		Scope<RenderPass> RenderPass;
		ViewportExtent ViewportExtent{};

		uint32_t CurrentFrameIndex = static_cast<uint32_t>(-1);

		CommandBuffer* ActiveCommandBuffer = nullptr;
		Scope<CommandBuffer> CommandBuffers[FramesInFlight];
	};

	static RendererData* s_Data = nullptr;

	void Renderer::BindPipeline(const Pipeline* pipeline)
	{
		QK_ASSERT_RENDER_THREAD();
		QK_CORE_ASSERT(s_Data->ActiveCommandBuffer->IsInsideRenderPass(), "Cannot bind a graphics pipeline outside of a render pass!");

		s_Data->ActiveCommandBuffer->BindPipeline(pipeline);
	}

	void Renderer::BeginRenderPass(const RenderPass* renderPass, const Framebuffer* framebuffer)
	{
		QK_ASSERT_RENDER_THREAD();
		QK_CORE_ASSERT(!s_Data->ActiveCommandBuffer->IsInsideRenderPass(), "Active command buffer is already inside a render pass!");

		s_Data->ActiveCommandBuffer->BeginRenderPass(renderPass, framebuffer);
	}

	void Renderer::BeginRenderPass()
	{
		BeginRenderPass(s_Data->RenderPass.get(), GetTargetFramebuffer());
	}

	void Renderer::EndRenderPass()
	{
		QK_ASSERT_RENDER_THREAD();
		QK_CORE_ASSERT(s_Data->ActiveCommandBuffer->IsInsideRenderPass(), "Active command buffer is not inside a render pass!");
		s_Data->ActiveCommandBuffer->EndRenderPass();
	}

	void Renderer::Draw(const VertexBuffer* vertexBuffer, uint32_t vertexCount)
	{
		QK_ASSERT_RENDER_THREAD();

		s_Data->ActiveCommandBuffer->BindVertexBuffer(vertexBuffer);
		s_Data->ActiveCommandBuffer->Draw(vertexCount, 0);
	}

	void Renderer::DrawIndexed(const VertexBuffer* vertexBuffer, const IndexBuffer* indexBuffer)
	{
		QK_ASSERT_RENDER_THREAD();

		s_Data->ActiveCommandBuffer->BindVertexBuffer(vertexBuffer);
		s_Data->ActiveCommandBuffer->BindIndexBuffer(indexBuffer);
		s_Data->ActiveCommandBuffer->DrawIndexed(indexBuffer->GetCount());
	}

	void Renderer::DrawIndexed(const VertexBuffer* vertexBuffer, const IndexBuffer* indexBuffer, uint32_t indexCount)
	{
		QK_ASSERT_RENDER_THREAD();

		s_Data->ActiveCommandBuffer->BindVertexBuffer(vertexBuffer);
		s_Data->ActiveCommandBuffer->BindIndexBuffer(indexBuffer);
		s_Data->ActiveCommandBuffer->DrawIndexed(indexCount);
	}

	void Renderer::BindTexture(const Texture* texture, const Sampler* sampler, uint32_t binding, uint32_t samplerIndex)
	{
		s_Data->ActiveCommandBuffer->BindTexture(texture, sampler, s_Data->CurrentFrameIndex, binding, samplerIndex);
	}

	void Renderer::BindUniformBuffer(const UniformBuffer* uniformBuffer, uint32_t binding)
	{
		s_Data->ActiveCommandBuffer->BindUniformBuffer(uniformBuffer, s_Data->CurrentFrameIndex, binding);
	}

	void Renderer::BindDescriptorSets()
	{
		s_Data->ActiveCommandBuffer->BindDescriptorSets(s_Data->CurrentFrameIndex);
	}

	void Renderer::SetLineWidth(float width)
	{
		s_Data->ActiveCommandBuffer->SetLineWidth(width);
	}

	void Renderer::SetViewport(uint32_t viewportWidth, uint32_t viewportHeight)
	{
		QK_ASSERT_RENDER_THREAD();

		if (viewportWidth != 0 && viewportHeight != 0)
		{
			s_Data->ViewportExtent.Width = viewportWidth;
			s_Data->ViewportExtent.Height = viewportHeight;
		}
	}

	void Renderer::SetClearColor(const Vec4f& clearColor)
	{
		QK_ASSERT_RENDER_THREAD();
		s_Data->RenderPass->SetClearColor(clearColor);
	}

	void Renderer::WaitUntilDeviceIdle()
	{
		QK_ASSERT_RENDER_THREAD();
		QK_PROFILE_FUNCTION();

		GraphicsContext::Get()->WaitUntilDeviceIdle();
	}

	const Vec4f& Renderer::GetClearColor()
	{
		return s_Data->RenderPass->GetSpecification().ClearColor;
	}

	const GraphicsAPICapabilities& Renderer::GetCapabilities()
	{
		return s_GraphicsAPI->GetCapabilities();
	}

	RenderPass* Renderer::GetRenderPass()
	{
		QK_ASSERT_RENDER_THREAD();
		return s_Data->RenderPass.get();
	}

	Framebuffer* Renderer::GetTargetFramebuffer()
	{
		QK_ASSERT_RENDER_THREAD();

		return GraphicsContext::Get()->GetFramebuffer();
	}

	ViewportExtent Renderer::GetViewportExtent()
	{
		return s_Data->ViewportExtent;
	}

	uint32_t Renderer::GetMultisampling()
	{
		return s_Data->RenderPass->GetSpecification().Samples;
	}

	uint32_t Renderer::GetFramesInFlight()
	{
		return RendererData::FramesInFlight;
	}

	uint32_t Renderer::GetCurrentFrameIndex()
	{
		return s_Data->CurrentFrameIndex;
	}

	CommandBuffer* Renderer::GetCommandBuffer()
	{
		QK_ASSERT_RENDER_THREAD();
		return s_Data->ActiveCommandBuffer;
	}

	ShaderLibrary& Renderer::GetShaderLibrary()
	{
		return s_Data->ShaderLib;
	}

	RHI Renderer::GetCurrentRHI()
	{
		return GraphicsAPI::GetAPI();
	}

	RHI Renderer::GetPreferredRHI()
	{
		return GraphicsAPI::GetDefaultRHIForPlatform();
	}

	RHIVersion Renderer::GetRHIVersion()
	{
		return s_GraphicsAPI->GetRHIVersion();
	}

	std::string Renderer::GetSpecification()
	{
		return s_GraphicsAPI->GetSpecification();
	}

	const char* Renderer::GetAPIName()
	{
		return s_GraphicsAPI->GetName();
	}

	void Renderer::BeginFrame()
	{
		QK_ASSERT_RENDER_THREAD();

		s_Data->CurrentFrameIndex = (s_Data->CurrentFrameIndex + 1) % RendererData::FramesInFlight;

		GraphicsContext::Get()->BeginFrame(s_Data->CurrentFrameIndex);

		s_Data->ActiveCommandBuffer = s_Data->CommandBuffers[s_Data->CurrentFrameIndex].get();
		s_Data->ActiveCommandBuffer->Reset();
		s_Data->ActiveCommandBuffer->Begin();
		s_Data->ActiveCommandBuffer->SetViewport(s_Data->ViewportExtent.Width, s_Data->ViewportExtent.Height);

		BeginRenderPass();
	}

	void Renderer::EndFrame()
	{
		QK_ASSERT_RENDER_THREAD();

		EndRenderPass();
		s_Data->ActiveCommandBuffer->End();

		GraphicsContext::Get()->Flush(s_Data->ActiveCommandBuffer, s_Data->CurrentFrameIndex);
	}

	void Renderer::Configure(RHI api)
	{
		s_GraphicsAPI = GraphicsAPI::Instantiate(api);
	}

	void Renderer::Initialize(GraphicsContext* context, uint32_t samples)
	{
		QK_PROFILE_FUNCTION();

		if (s_Data) return;

		s_ThreadId = std::this_thread::get_id();

		context->Init();

		s_GraphicsAPI->Init();

		s_Data = new RendererData();
		s_Data->ViewportExtent = context->GetViewportExtent();

		SwapSurfaceFormat targetSurfaceFormat{};
		targetSurfaceFormat.Format = ColorFormat::BGRA8SRGB;
		targetSurfaceFormat.ColorSpace = ColorSpace::SRGBNonLinear;

		SwapSurfaceFormat surfaceFormat = context->ChooseSurfaceFormat(targetSurfaceFormat);

		{
			RenderPassSpecification spec;
			spec.BindPoint             = PipelineBindPoint::Graphics;
			spec.ColorAttachmentFormat = surfaceFormat.Format;
			spec.DepthAttachmentFormat = ColorFormat::Depth32f;
			spec.ClearColor            = { 0.01f, 0.01f, 0.01f, 1.0f };
			spec.ClearBuffers          = true;
			spec.Samples               = samples;
			s_Data->RenderPass = RenderPass::Create(spec);
		}

		SwapChainSpecification swapChainSpec;
		swapChainSpec.MinImageCount = context->GetSwapChainImageCount();
		swapChainSpec.Extent        = context->ChooseSwapExtent(s_Data->ViewportExtent.Width, s_Data->ViewportExtent.Height);
		swapChainSpec.SurfaceFormat = surfaceFormat;
		swapChainSpec.PresentMode   = context->ChooseSwapPresentMode(SwapPresentMode::Mailbox);
		swapChainSpec.RenderPass    = s_Data->RenderPass.get();

		context->CreateSwapChain(swapChainSpec);

		for (uint32_t i = 0; i < RendererData::FramesInFlight; i++)
			s_Data->CommandBuffers[i] = CommandBuffer::Create();

		s_Data->ActiveCommandBuffer = s_Data->CommandBuffers[0].get();
	}

	void Renderer::Dispose()
	{
		QK_PROFILE_FUNCTION();

		delete s_Data;
		s_Data = nullptr;
	}
}
