#pragma once

#include "Quark/Renderer/Pipeline.h"

namespace Quark {

	class OpenGLPipeline final : public Pipeline
	{
	public:
		OpenGLPipeline(const PipelineSpecification& spec);
		virtual ~OpenGLPipeline() override;

		virtual void BeginFrame() override;
		virtual void EndFrame() override;

		virtual void BeginRenderPass(const Ref<RenderPass>& renderPass) override;
		virtual void EndRenderPass() override;

		virtual void Resize(uint32_t viewportWidth, uint32_t viewportHeight) override;

		virtual const Ref<CommandBuffer>& GetCommandBuffer() const override { return m_CommandBuffer; }
		virtual const Ref<UniformBuffer>& GetUniformBuffer() const override { return m_UniformBuffer; }

	private:
		Ref<CommandBuffer> m_CommandBuffer;
		Ref<UniformBuffer> m_UniformBuffer;
	};
}
