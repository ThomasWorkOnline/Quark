#pragma once

#include "Quark/Core/Core.h"
#include "RenderModes.h"

#include "Buffer.h"
#include "CommandBuffer.h"
#include "RenderPass.h"
#include "Shader.h"
#include "UniformBuffer.h"

namespace Quark {

	struct PipelineSpecification
	{
		uint32_t            ViewportWidth = 0, ViewportHeight = 0;
		uint32_t            FramesInFlight = 1;
		size_t              CameraUniformBufferSize = 0;

		RenderCullMode      CullMode = RenderCullMode::Default;
		RenderDepthFunction DepthFunction = RenderDepthFunction::Default;

		BufferLayout        Layout;
		Ref<RenderPass>     RenderPass;
		Ref<Shader>         VertexShader;
		Ref<Shader>         FragmentShader;
	};

	class Pipeline
	{
	public:
		virtual ~Pipeline() = default;

		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;

		virtual void BeginRenderPass(const Ref<RenderPass>& renderPass) = 0;
		virtual void EndRenderPass() = 0;

		virtual const Ref<CommandBuffer>& GetCommandBuffer() const = 0;
		virtual const Ref<UniformBuffer>& GetUniformBuffer() const = 0;

		virtual void Submit() = 0;
		virtual void Resize(uint32_t viewportWidth, uint32_t viewportHeight) = 0;

		virtual const PipelineSpecification& GetSpecification() const = 0;

		static Ref<Pipeline> Create(const PipelineSpecification& spec);
	};
}
