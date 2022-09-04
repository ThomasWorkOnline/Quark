#include "qkpch.h"
#include "Renderer.h"

#include "Quark/Filesystem/Filesystem.h"

#include "CommandBuffer.h"
#include "GraphicsContext.h"

#include "GraphicsAPI.cpp"

namespace Quark {

	struct RendererData
	{
		ShaderLibrary ShaderLib;
		Scope<RenderPass> GeometryPass;

		CommandBuffer* ActiveCommandBuffer = nullptr;
		std::vector<Scope<Framebuffer>> Framebuffers;
	};

	static RendererData* s_Data = nullptr;

	void Renderer::BindPipeline(Pipeline* pipeline)
	{
		QK_ASSERT_RENDER_THREAD();
		s_Data->ActiveCommandBuffer->BindPipeline(pipeline);
	}

	void Renderer::BeginRenderPass(RenderPass* renderPass, Framebuffer* framebuffer)
	{
		QK_ASSERT_RENDER_THREAD();
		QK_CORE_ASSERT(!s_Data->ActiveCommandBuffer->IsInsideRenderPass(), "Active command buffer is already inside a render pass!");

		s_Data->ActiveCommandBuffer->BeginRenderPass(renderPass, framebuffer);
	}

	void Renderer::BeginGeometryPass()
	{
		QK_ASSERT_RENDER_THREAD();
		s_Data->ActiveCommandBuffer->BeginRenderPass(s_Data->GeometryPass.get(), GetTargetFramebuffer());
	}

	void Renderer::EndRenderPass()
	{
		QK_ASSERT_RENDER_THREAD();
		s_Data->ActiveCommandBuffer->EndRenderPass();
	}

	void Renderer::Submit(VertexBuffer* vertexBuffer, uint32_t vertexCount)
	{
		QK_ASSERT_RENDER_THREAD();

		s_Data->ActiveCommandBuffer->BindVertexBuffer(vertexBuffer);
		s_Data->ActiveCommandBuffer->Draw(vertexCount, 0);
	}

	void Renderer::Submit(VertexBuffer* vertexBuffer, IndexBuffer* indexBuffer)
	{
		QK_ASSERT_RENDER_THREAD();

		s_Data->ActiveCommandBuffer->BindVertexBuffer(vertexBuffer);
		s_Data->ActiveCommandBuffer->BindIndexBuffer(indexBuffer);
		s_Data->ActiveCommandBuffer->DrawIndexed(indexBuffer->GetCount());
	}

	void Renderer::Submit(VertexBuffer* vertexBuffer, IndexBuffer* indexBuffer, uint32_t indexCount)
	{
		QK_ASSERT_RENDER_THREAD();

		s_Data->ActiveCommandBuffer->BindVertexBuffer(vertexBuffer);
		s_Data->ActiveCommandBuffer->BindIndexBuffer(indexBuffer);
		s_Data->ActiveCommandBuffer->DrawIndexed(indexCount);
	}

	void Renderer::SetViewport(uint32_t viewportWidth, uint32_t viewportHeight)
	{
		QK_ASSERT_RENDER_THREAD();

		GraphicsContext::Get()->Resize(viewportWidth, viewportHeight);

		if (GraphicsAPI::GetAPI() == RHI::Vulkan)
		{
			for (auto& framebuffer : s_Data->Framebuffers)
			{
				framebuffer->Resize(viewportWidth, viewportHeight);
			}
		}

		if (viewportWidth != 0 && viewportHeight != 0)
		{
			s_ViewportExtent.Width = viewportWidth;
			s_ViewportExtent.Height = viewportHeight;
		}
	}

	void Renderer::SetClearColor(const Vec4f& clearColor)
	{
		QK_ASSERT_RENDER_THREAD();
		s_Data->GeometryPass->SetClearColor(clearColor);
	}

	void Renderer::WaitUntilDeviceIdle()
	{
		QK_ASSERT_RENDER_THREAD();
		QK_PROFILE_FUNCTION();

		GraphicsContext::Get()->WaitUntilDeviceIdle();
	}

	const Vec4f& Renderer::GetClearColor()
	{
		return s_Data->GeometryPass->GetSpecification().ClearColor;
	}

	const GraphicsAPICapabilities& Renderer::GetCapabilities()
	{
		return s_GraphicsAPI->GetCapabilities();
	}

	RenderPass* Renderer::GetGeometryPass()
	{
		QK_ASSERT_RENDER_THREAD();
		return s_Data->GeometryPass.get();
	}

	Framebuffer* Renderer::GetTargetFramebuffer()
	{
		QK_ASSERT_RENDER_THREAD();

		const uint32_t imageIndex = GraphicsContext::Get()->GetCurrentImageIndex();
		return s_Data->Framebuffers[imageIndex].get();
	}

	CommandBuffer* Renderer::GetCommandBuffer()
	{
		QK_ASSERT_RENDER_THREAD();
		return s_Data->ActiveCommandBuffer;
	}

	Renderer::ViewportExtent Renderer::GetViewportExtent()
	{
		return s_ViewportExtent;
	}

	ShaderLibrary& Renderer::GetShaderLibrary()
	{
		return s_Data->ShaderLib;
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

		GraphicsContext::Get()->BeginFrame();

		s_Data->ActiveCommandBuffer = GraphicsContext::Get()->GetCommandBuffer();
		s_Data->ActiveCommandBuffer->Reset();
		s_Data->ActiveCommandBuffer->Begin();
		s_Data->ActiveCommandBuffer->SetViewport(s_ViewportExtent.Width, s_ViewportExtent.Height);

		BeginGeometryPass();
	}

	void Renderer::EndFrame()
	{
		QK_ASSERT_RENDER_THREAD();

		EndRenderPass();

		s_Data->ActiveCommandBuffer->End();
		GraphicsContext::Get()->EndFrame();
	}

	void Renderer::Configure(RHI api)
	{
		s_GraphicsAPI = GraphicsAPI::Instantiate(api);
	}

	void Renderer::Initialize(uint32_t viewportWidth, uint32_t viewportHeight)
	{
		QK_PROFILE_FUNCTION();

		if (s_Data) return;

		s_ViewportExtent.Width = viewportWidth;
		s_ViewportExtent.Height = viewportHeight;
		s_ThreadId = std::this_thread::get_id();

		s_GraphicsAPI->Init();
		s_Data = new RendererData();

		s_Data->ActiveCommandBuffer = GraphicsContext::Get()->GetCommandBuffer();

		{
			RenderPassSpecification spec;
			spec.BindPoint = PipelineBindPoint::Graphics;
			spec.ColorFormat = ColorDataFormat::BGRA8_SRGB;
			spec.ClearColor = { 0.01f, 0.01f, 0.01f, 1.0f };
			spec.ClearBuffers = true;
			s_Data->GeometryPass = RenderPass::Create(spec);
		}

		const uint32_t swapChainImages = GraphicsContext::Get()->GetSwapChainImageCount();
		s_Data->Framebuffers.resize(swapChainImages);

		// Target the default framebuffer when using OpenGL
		if (GraphicsAPI::GetAPI() == RHI::Vulkan)
		{
			FramebufferSpecification fbSpec;
			fbSpec.Width = s_ViewportExtent.Width;
			fbSpec.Height = s_ViewportExtent.Height;
			fbSpec.RenderPass = s_Data->GeometryPass.get();
			fbSpec.SwapChainTarget = true;

			for (uint32_t i = 0; i < swapChainImages; i++)
			{
				s_Data->Framebuffers[i] = Framebuffer::Create(fbSpec);
			}
		}
	}

	void Renderer::Dispose()
	{
		QK_PROFILE_FUNCTION();

		delete s_Data;
		s_Data = nullptr;
	}
}
