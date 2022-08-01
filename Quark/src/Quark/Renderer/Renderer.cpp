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
		Scope<BackendRenderer> Backend;
	};

	static Scope<RendererData> s_Data;

	void Renderer::Initialize()
	{
		QK_PROFILE_FUNCTION();

		s_Data = CreateScope<RendererData>();
		s_Data->MaxUniformBuffers = GraphicsAPI::Instance->GetHardwareConstraints().UniformBufferConstraints.MaxBindings;
		s_Data->Backend = BackendRenderer::Create();
		s_Data->Backend->Initialize();
	}

	void Renderer::Dispose()
	{
		QK_PROFILE_FUNCTION();
		s_Data.reset();
	}

	void Renderer::BeginFrame()
	{
		s_Data->Backend->BeginFrame();

		s_Data->ActiveCommandBuffer = GetCommandBuffer().get();
		s_Data->ActiveCommandBuffer->Reset();
		s_Data->ActiveCommandBuffer->Begin();
	}

	void Renderer::EndFrame()
	{
		s_Data->ActiveCommandBuffer->End();
		s_Data->Backend->Submit();
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
		return s_Data->Backend->GetCommandBuffer();
	}

	void Renderer::OnViewportResized(uint32_t width, uint32_t height)
	{
		s_Data->Backend->OnViewportResized(width, height);
	}

	ShaderLibrary& Renderer::GetShaderLibrary()
	{
		return s_Data->ShaderLib;
	}
}

#include "Platform/OpenGL/OpenGLRenderer.h"
#include "Platform/Vulkan/VulkanRenderer.h"

namespace Quark {

	Scope<BackendRenderer> BackendRenderer::Create()
	{
		switch (GraphicsAPI::GetAPI())
		{
			case API::OpenGL: return CreateScope<OpenGLRenderer>();
			case API::Vulkan: return CreateScope<VulkanRenderer>();
			default:
				QK_CORE_FATAL("No default renderer");
				return nullptr;
		}
	}
}
