#pragma once

#include "Quark/Core/Core.h"

#include "APICapabilities.h"

#include "Buffer.h"
#include "CommandBuffer.h"
#include "Formats.h"
#include "Framebuffer.h"
#include "Pipeline.h"
#include "RenderPass.h"
#include "Shader.h"

#include <thread>

#define QK_ASSERT_RENDER_THREAD() QK_CORE_ASSERT(std::this_thread::get_id() == ::Quark::Renderer::GetThreadId(), "Function must be called from the render thread!")

namespace Quark {

	struct ViewportExtent
	{
		uint32_t Width, Height;
	};

	class Renderer
	{
	public:
		static void BindPipeline(const Pipeline* pipeline);

		static void BeginRenderPass(const RenderPass* renderPass, const Framebuffer* framebuffer);
		static void BeginRenderPass();
		static void EndRenderPass();

		static void Submit(const VertexBuffer* vertexBuffer, uint32_t vertexCount);
		static void Submit(const VertexBuffer* vertexBuffer, const IndexBuffer* indexBuffer);
		static void Submit(const VertexBuffer* vertexBuffer, const IndexBuffer* indexBuffer, uint32_t indexCount);

		static void SetViewport(uint32_t viewportWidth, uint32_t viewportHeight);
		static void SetClearColor(const Vec4f& clearColor);
		static void WaitUntilDeviceIdle();

		static const Vec4f&    GetClearColor();
		static const GraphicsAPICapabilities& GetCapabilities();

		static RenderPass*     GetRenderPass();

		static Framebuffer*    GetTargetFramebuffer();
		static ViewportExtent  GetViewportExtent();
		static uint32_t        GetMultisampling();

		static CommandBuffer*  GetCommandBuffer();

		static ShaderLibrary&  GetShaderLibrary();
		static std::thread::id GetThreadId() { return s_ThreadId; }

		static RHI             GetCurrentRHI();
		static RHI             GetPreferredRHI();
		static RHIVersion      GetRHIVersion();
		static std::string     GetSpecification();
		static const char*     GetAPIName();

	private:
		static void BeginFrame();
		static void EndFrame();

		static void Configure(RHI api);
		static void Initialize(uint32_t viewportWidth, uint32_t viewportHeight, uint32_t samples);
		static void Dispose();

	private:
		static inline std::thread::id s_ThreadId;
		friend class Application;
	};
}
