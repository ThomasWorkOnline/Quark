#pragma once

#include "Quark/Core/Core.h"

#include "Buffer.h"
#include "Camera.h"
#include "CommandBuffer.h"
#include "Framebuffer.h"
#include "Pipeline.h"
#include "RenderPass.h"
#include "Shader.h"
#include "Texture.h"

#include <thread>

#define QK_ASSERT_RENDER_THREAD() QK_CORE_ASSERT(std::this_thread::get_id() == ::Quark::Renderer::GetThreadId(), "Function must be called from the render thread!")

namespace Quark {

	class Renderer
	{
	public:
		static void BindPipeline(const Ref<Pipeline>& pipeline);
		static void BeginRenderPass(const Ref<RenderPass>& renderPass, const Ref<Framebuffer>& framebuffer);
		static void EndRenderPass();

		static void Submit(const Ref<VertexBuffer>& vertexBuffer, const Ref<IndexBuffer>& indexBuffer);
		static void Submit(const Ref<VertexBuffer>& vertexBuffer, const Ref<IndexBuffer>& indexBuffer, uint32_t indexCount);
		static void DrawLines(const Ref<VertexBuffer>& vertexBuffer, uint32_t vertexCount);

		static const Ref<CommandBuffer>& GetCommandBuffer();
		static ShaderLibrary& GetShaderLibrary();

		static std::thread::id GetThreadId() { return s_ThreadId; }

	private:
		static void BeginFrame();
		static void EndFrame();

		static void OnViewportResized(uint32_t width, uint32_t height);

		static void Initialize();
		static void Dispose();

	private:
		static inline std::thread::id s_ThreadId;

		friend class Application;
	};
}
