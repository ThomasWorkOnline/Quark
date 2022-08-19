#include "qkpch.h"
#include "Renderer.h"

#include "CommandBuffer.h"
#include "GraphicsContext.h"
#include "RenderCommand.h"

#include "Quark/Filesystem/Filesystem.h"

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

	void Renderer::Submit(VertexBuffer* vertexBuffer, IndexBuffer* indexBuffer)
	{
		QK_ASSERT_RENDER_THREAD();

		s_Data->ActiveCommandBuffer->BindVertexBuffer(vertexBuffer);
		s_Data->ActiveCommandBuffer->BindIndexBuffer(indexBuffer);
		s_Data->ActiveCommandBuffer->SetPrimitiveTopology(PrimitiveTopology::TriangleList);
		s_Data->ActiveCommandBuffer->DrawIndexed(indexBuffer->GetCount());
	}

	void Renderer::Submit(VertexBuffer* vertexBuffer, IndexBuffer* indexBuffer, uint32_t indexCount)
	{
		QK_ASSERT_RENDER_THREAD();

		s_Data->ActiveCommandBuffer->BindVertexBuffer(vertexBuffer);
		s_Data->ActiveCommandBuffer->BindIndexBuffer(indexBuffer);
		s_Data->ActiveCommandBuffer->SetPrimitiveTopology(PrimitiveTopology::TriangleList);
		s_Data->ActiveCommandBuffer->DrawIndexed(indexCount);
	}

	void Renderer::DrawLines(VertexBuffer* vertexBuffer, uint32_t vertexCount)
	{
		QK_ASSERT_RENDER_THREAD();

		s_Data->ActiveCommandBuffer->BindVertexBuffer(vertexBuffer);
		s_Data->ActiveCommandBuffer->SetPrimitiveTopology(PrimitiveTopology::LineList);
		s_Data->ActiveCommandBuffer->Draw(vertexCount, 0);
	}

	void Renderer::SetViewport(uint32_t viewportWidth, uint32_t viewportHeight)
	{
		QK_ASSERT_RENDER_THREAD();

		s_ViewportExtent.Width = viewportWidth;
		s_ViewportExtent.Height = viewportHeight;
		s_Data->ActiveCommandBuffer->SetViewport(viewportWidth, viewportHeight);

		GraphicsContext::Get()->Resize(viewportWidth, viewportHeight);
	}

	RenderPass* Renderer::GetGeometryPass()
	{
		return s_Data->GeometryPass.get();
	}

	Framebuffer* Renderer::GetTargetFramebuffer()
	{
		const uint32_t imageIndex = GraphicsContext::Get()->GetCurrentImageIndex();
		return s_Data->Framebuffers[imageIndex].get();
	}

	CommandBuffer* Renderer::GetCommandBuffer()
	{
		QK_ASSERT_RENDER_THREAD();
		return GraphicsContext::Get()->GetCommandBuffer();
	}

	Renderer::ViewportExtent Renderer::GetViewportExtent()
	{
		return s_ViewportExtent;
	}

	ShaderLibrary& Renderer::GetShaderLibrary()
	{
		return s_Data->ShaderLib;
	}

	void Renderer::BeginFrame()
	{
		QK_ASSERT_RENDER_THREAD();

		GraphicsContext::Get()->StartFrame();

		s_Data->ActiveCommandBuffer = GetCommandBuffer();
		s_Data->ActiveCommandBuffer->Reset();
		s_Data->ActiveCommandBuffer->Begin();

		BeginGeometryPass();
	}

	void Renderer::EndFrame()
	{
		QK_ASSERT_RENDER_THREAD();

		EndRenderPass();

		s_Data->ActiveCommandBuffer->End();
		GraphicsContext::Get()->Submit();
	}

	void Renderer::Initialize(uint32_t viewportWidth, uint32_t viewportHeight)
	{
		QK_PROFILE_FUNCTION();

		if (s_Data) return;

		s_ViewportExtent.Width = viewportWidth;
		s_ViewportExtent.Height = viewportHeight;

		s_ThreadId = std::this_thread::get_id();
		RenderCommand::Init();

		s_Data = new RendererData();

#if 0
		std::string vertexSource = Filesystem::ReadTextFile("../Quark/assets/shaders/version/4.50/bin/shader.vert.spv");
		std::string fragmentSource = Filesystem::ReadTextFile("../Quark/assets/shaders/version/4.50/bin/shader.frag.spv");

		s_Data->Shader = Shader::Create("shader", vertexSource, fragmentSource);
#endif

		{
			RenderPassSpecification spec;
			spec.BindPoint = PipelineBindPoint::Graphics;
			spec.ColorFormat = ColorDataFormat::BGRA8_SRGB;
			spec.ClearBuffers = true;
			s_Data->GeometryPass = RenderPass::Create(spec);
		}

		const uint32_t swapChainImages = GraphicsContext::Get()->GetSwapChainImageCount();
		s_Data->Framebuffers.resize(swapChainImages);

#if 0
		// Target the default framebuffer when using OpenGL
		if (GraphicsAPI::GetAPI() == RHI::Vulkan)
		{
			FramebufferSpecification fbSpec;
			fbSpec.Width = s_ViewportExtent.Width;
			fbSpec.Height = s_ViewportExtent.Height;
			fbSpec.SwapChainTarget = true;
			fbSpec.RenderPass = s_Data->GeometryPass.get();

			for (uint32_t i = 0; i < swapChainImages; i++)
			{
				s_Data->Framebuffers[i].reset(Framebuffer::Create(fbSpec));
			}
		}
#endif
	}

	void Renderer::Dispose()
	{
		QK_PROFILE_FUNCTION();

		delete s_Data;
		s_Data = nullptr;
	}
}
