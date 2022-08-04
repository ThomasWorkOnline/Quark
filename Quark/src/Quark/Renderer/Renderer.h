#pragma once

#include "Quark/Core/Core.h"

#include "Camera.h"
#include "CommandBuffer.h"
#include "Framebuffer.h"
#include "Pipeline.h"
#include "RenderPass.h"
#include "Shader.h"
#include "Texture.h"

namespace Quark {

	class Renderer
	{
	public:
		static void BeginFrame();
		static void EndFrame();

		static void BindPipeline(const Ref<Pipeline>& pipeline);
		static void BeginRenderPass(const Ref<RenderPass>& renderPass, const Ref<Framebuffer>& framebuffer);
		static void EndRenderPass();

		static void OnViewportResized(uint32_t width, uint32_t height);

		static const Ref<CommandBuffer>& GetCommandBuffer();
		static ShaderLibrary& GetShaderLibrary();

	private:
		static void Initialize();
		static void Dispose();

		friend class Application;
	};
}
