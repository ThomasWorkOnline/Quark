#pragma once

#include "Quark/Core/Core.h"

#include "APICapabilities.h"

#include "Buffer.h"
#include "CommandBuffer.h"
#include "Framebuffer.h"
#include "Pipeline.h"
#include "RenderPass.h"
#include "Shader.h"

#include <thread>

#define QK_ASSERT_RENDER_THREAD() QK_CORE_ASSERT(std::this_thread::get_id() == ::Quark::Renderer::GetThreadId(), "Function must be called from the render thread!")

namespace Quark {

	class Renderer
	{
	public:
		struct ViewportExtent
		{
			uint32_t Width, Height;
		};

		static void BindPipeline(Pipeline* pipeline);

		static void BeginRenderPass(RenderPass* renderPass, Framebuffer* framebuffer);
		static void BeginGeometryPass();
		static void EndRenderPass();

		static void Submit(VertexBuffer* vertexBuffer, uint32_t vertexCount);
		static void Submit(VertexBuffer* vertexBuffer, IndexBuffer* indexBuffer);
		static void Submit(VertexBuffer* vertexBuffer, IndexBuffer* indexBuffer, uint32_t indexCount);

		static void SetViewport(uint32_t viewportWidth, uint32_t viewportHeight);
		static void SetClearColor(const Vec4f& clearColor);
		static void WaitUntilDeviceIdle();

		static const Vec4f& GetClearColor();
		static const GraphicsAPICapabilities& GetCapabilities();

		static RenderPass* GetGeometryPass();
		static Framebuffer* GetTargetFramebuffer();
		static CommandBuffer* GetCommandBuffer();
		static ViewportExtent GetViewportExtent();

		static ShaderLibrary& GetShaderLibrary();
		static std::thread::id GetThreadId() { return s_ThreadId; }

		static RHI GetPreferredRHI();
		static std::string GetSpecification();
		static const char* GetAPIName();

	private:
		static void BeginFrame();
		static void EndFrame();

		static void Configure(RHI api);
		static void Initialize(uint32_t viewportWidth, uint32_t viewportHeight);
		static void Dispose();

	private:
		static inline std::thread::id s_ThreadId;
		static inline ViewportExtent s_ViewportExtent{};

		friend class Application;
	};
}
