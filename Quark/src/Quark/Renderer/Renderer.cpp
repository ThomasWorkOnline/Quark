#include "qkpch.h"
#include "Renderer.h"

#include "Quark/Filesystem/Filesystem.h"

#include "GraphicsAPI.cpp"

namespace Quark {

	struct RendererData
	{
		uint32_t FramesInFlight = 1;

		ShaderLibrary     ShaderLib;
		Scope<RenderPass> RenderPass;
		ViewportExtent    ViewportExtent{};

		uint32_t CurrentFrameIndex = ~0U;

		const Pipeline* BoundPipeline = nullptr;
		CommandBuffer*  ActiveCommandBuffer = nullptr;

		std::vector<Scope<CommandBuffer>> CommandBuffers;
		std::vector<Scope<Framebuffer>>   Framebuffers;

		Scope<FramebufferAttachment> ResolveBuffer;
		Scope<FramebufferAttachment> DepthBuffer;
	};

	static RendererData* s_Data = nullptr;

	void Renderer::BindPipeline(const Pipeline* pipeline)
	{
		QK_ASSERT_RENDER_THREAD();
		QK_CORE_ASSERT(s_Data->ActiveCommandBuffer->IsInsideRenderPass(), "Cannot bind a graphics pipeline outside of a render pass!");

		s_Data->BoundPipeline = pipeline;
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
		BeginRenderPass(s_Data->RenderPass.get(), GetFramebuffer());
	}

	void Renderer::EndRenderPass()
	{
		QK_ASSERT_RENDER_THREAD();
		QK_CORE_ASSERT(s_Data->ActiveCommandBuffer->IsInsideRenderPass(), "Active command buffer is not inside a render pass!");
		s_Data->ActiveCommandBuffer->EndRenderPass();
	}

	void Renderer::PushConstant(ShaderStage stage, const void* data, size_t size)
	{
		QK_ASSERT_RENDER_THREAD();

		s_Data->ActiveCommandBuffer->PushConstant(s_Data->BoundPipeline, stage, data, size);
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
		s_Data->ActiveCommandBuffer->BindTexture(s_Data->BoundPipeline, texture, sampler, s_Data->CurrentFrameIndex, binding, samplerIndex);
	}

	void Renderer::BindUniformBuffer(const UniformBuffer* uniformBuffer, uint32_t binding)
	{
		s_Data->ActiveCommandBuffer->BindUniformBuffer(s_Data->BoundPipeline, uniformBuffer, s_Data->CurrentFrameIndex, binding);
	}

	void Renderer::BindDescriptorSets()
	{
		s_Data->ActiveCommandBuffer->BindDescriptorSets(s_Data->BoundPipeline, s_Data->CurrentFrameIndex);
	}

	void Renderer::SetLineWidth(float width)
	{
		s_Data->ActiveCommandBuffer->SetLineWidth(width);
	}

	void Renderer::SetViewport(uint32_t viewportWidth, uint32_t viewportHeight)
	{
		QK_ASSERT_RENDER_THREAD();

		if (viewportWidth == 0 || viewportHeight == 0)
			return;

		s_Data->ViewportExtent.Width = viewportWidth;
		s_Data->ViewportExtent.Height = viewportHeight;

		if (s_Data->DepthBuffer)
			s_Data->DepthBuffer->Resize(viewportWidth, viewportHeight);

		if (s_Data->ResolveBuffer)
			s_Data->ResolveBuffer->Resize(viewportWidth, viewportHeight);

		for (size_t i = 0; i < s_Data->Framebuffers.size(); i++)
		{
			s_Data->Framebuffers[i]->Resize(viewportWidth, viewportHeight);
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

	Framebuffer* Renderer::GetFramebuffer()
	{
		QK_ASSERT_RENDER_THREAD();
		uint32_t imageIndex = GraphicsContext::Get()->GetSwapChain()->GetCurrentImageIndex();
		return s_Data->Framebuffers[imageIndex].get();
	}

	SampleCount Renderer::GetSupportedMSAA()
	{
		return s_GraphicsAPI->GetCapabilities().Framebuffer.MaxMSAA;
	}

	ViewportExtent Renderer::GetViewportExtent()
	{
		return s_Data->ViewportExtent;
	}

	uint32_t Renderer::GetFramesInFlight()
	{
		return s_Data->FramesInFlight;
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

	const char* Renderer::GetDeviceName()
	{
		return s_GraphicsAPI->GetDeviceName();
	}

	void Renderer::BeginFrame()
	{
		QK_ASSERT_RENDER_THREAD();

		s_Data->CurrentFrameIndex = (s_Data->CurrentFrameIndex + 1) % s_Data->FramesInFlight;

		GraphicsContext::Get()->BeginFrame(s_Data->CurrentFrameIndex);

		s_Data->ActiveCommandBuffer = s_Data->CommandBuffers[s_Data->CurrentFrameIndex].get();
		s_Data->ActiveCommandBuffer->Reset();
		s_Data->ActiveCommandBuffer->Begin();
		s_Data->ActiveCommandBuffer->SetViewport(s_Data->ViewportExtent.Width, s_Data->ViewportExtent.Height);
		s_Data->ActiveCommandBuffer->SetDepthFunction(DepthCompareFunction::Default);
		s_Data->ActiveCommandBuffer->SetCullMode(RenderCullMode::Default);

		BeginRenderPass();
	}

	void Renderer::EndFrame()
	{
		QK_ASSERT_RENDER_THREAD();

		EndRenderPass();
		s_Data->ActiveCommandBuffer->End();

		GraphicsContext::Get()->Flush(s_Data->ActiveCommandBuffer);
	}

	void Renderer::Configure(RHI api)
	{
		s_GraphicsAPI = GraphicsAPI::Instantiate(api);
	}

	void Renderer::Initialize(const SwapChain* swapChain)
	{
		QK_PROFILE_FUNCTION();

		if (s_Data) return;

		s_ThreadId = std::this_thread::get_id();
		s_GraphicsAPI->Init();

		s_Data = new RendererData();
		s_Data->ViewportExtent = swapChain->GetViewportExtent();

		{
			ColorFormat surfaceFormat  = swapChain->GetSpecification().SurfaceFormat.Format;
			SampleCount samples        = swapChain->GetSpecification().Samples;

			RenderPassSpecification spec;
			spec.BindPoint             = PipelineBindPoint::Graphics;
			spec.ColorAttachmentFormat = surfaceFormat;
			spec.DepthAttachmentFormat = ColorFormat::Depth32f;
			spec.ClearColor            = { 0.01f, 0.01f, 0.01f, 1.0f };
			spec.ClearBuffers          = true;
			spec.Samples               = samples;

			s_Data->RenderPass = RenderPass::Create(spec);
		}

		const uint32_t bufferCount = swapChain->GetBufferCount();

		s_Data->FramesInFlight = std::min(bufferCount, 1u);
		s_Data->CommandBuffers.resize(s_Data->FramesInFlight);
		s_Data->Framebuffers.resize(bufferCount);

		for (uint32_t i = 0; i < s_Data->FramesInFlight; i++)
			s_Data->CommandBuffers[i] = CommandBuffer::Create();

		s_Data->ActiveCommandBuffer = s_Data->CommandBuffers[0].get();

		// Resolve buffer
		if (s_Data->RenderPass->GetSpecification().Samples > SampleCount::SampleCount1)
		{
			FramebufferAttachmentSpecification resolveAttachmentSpec;
			resolveAttachmentSpec.Width = s_Data->ViewportExtent.Width;
			resolveAttachmentSpec.Height = s_Data->ViewportExtent.Height;
			resolveAttachmentSpec.Samples = s_Data->RenderPass->GetSpecification().Samples;
			resolveAttachmentSpec.DataFormat = s_Data->RenderPass->GetSpecification().ColorAttachmentFormat;

			s_Data->ResolveBuffer = FramebufferAttachment::Create(resolveAttachmentSpec);
		}

		// Depth buffer
		if (s_Data->RenderPass->GetSpecification().DepthAttachmentFormat != ColorFormat::None)
		{
			FramebufferAttachmentSpecification depthAttachmentSpec;
			depthAttachmentSpec.Width = s_Data->ViewportExtent.Width;
			depthAttachmentSpec.Height = s_Data->ViewportExtent.Height;
			depthAttachmentSpec.Samples = s_Data->RenderPass->GetSpecification().Samples;
			depthAttachmentSpec.DataFormat = s_Data->RenderPass->GetSpecification().DepthAttachmentFormat;

			s_Data->DepthBuffer = FramebufferAttachment::Create(depthAttachmentSpec);
		}

		for (uint32_t i = 0; i < bufferCount; i++)
		{
			FramebufferSpecification fbSpec;
			fbSpec.Width = s_Data->ViewportExtent.Width;
			fbSpec.Height = s_Data->ViewportExtent.Height;
			fbSpec.RenderPass = s_Data->RenderPass.get();
			fbSpec.SwapChainTarget = true;

			if (s_Data->ResolveBuffer)
			{
				auto&& resolveAttachment = fbSpec.Attachments.emplace_back();
				resolveAttachment = s_Data->ResolveBuffer.get();
			}

			auto&& colorAttachment = fbSpec.Attachments.emplace_back();
			colorAttachment = swapChain->GetColorAttachment(i);

			if (s_Data->DepthBuffer)
			{
				auto&& depthAttachment = fbSpec.Attachments.emplace_back();
				depthAttachment = s_Data->DepthBuffer.get();
			}

			s_Data->Framebuffers[i] = Framebuffer::Create(fbSpec);
		}
	}

	void Renderer::Dispose()
	{
		QK_PROFILE_FUNCTION();

		delete s_Data;
		s_Data = nullptr;
	}
}
