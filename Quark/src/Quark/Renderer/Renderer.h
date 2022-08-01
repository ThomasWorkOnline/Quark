#pragma once

#include "Quark/Core/Core.h"

#include "Camera.h"
#include "CommandBuffer.h"
#include "Framebuffer.h"
#include "RenderPass.h"
#include "Shader.h"
#include "Texture.h"

namespace Quark {

	class Renderer
	{
	public:
		static void BeginFrame();
		static void EndFrame();

		static void BeginRenderPass(const Ref<RenderPass>& renderPass, const Ref<Framebuffer>& framebuffer);
		static void EndRenderPass();

		static const Ref<CommandBuffer>& GetCommandBuffer();
		static void OnViewportResized(uint32_t width, uint32_t height);

		static ShaderLibrary& GetShaderLibrary();

	private:
		static void Initialize();
		static void Dispose();

		friend class Application;
	};

	class BackendRenderer
	{
	public:
		virtual ~BackendRenderer() = default;

		virtual void Initialize() = 0;
		virtual void Shutdown() = 0;

		virtual void BeginFrame() = 0;
		virtual void Submit() = 0;
		virtual const Ref<CommandBuffer>& GetCommandBuffer() const = 0;

		virtual void OnViewportResized(uint32_t viewportWidth, uint32_t viewportHeight) = 0;

		static Scope<BackendRenderer> Create();
	};
}
