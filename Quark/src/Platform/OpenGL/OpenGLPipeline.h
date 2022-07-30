#pragma once

#include "Quark/Renderer/Pipeline.h"

namespace Quark {

	class OpenGLPipeline final : public Pipeline
	{
	public:
		OpenGLPipeline(const PipelineSpecification& spec);
		virtual ~OpenGLPipeline() override;

		virtual void Bind(const Ref<CommandBuffer>& commandBuffer) override;
		virtual void Resize(uint32_t viewportWidth, uint32_t viewportHeight) override;
		virtual const Ref<UniformBuffer>& GetUniformBuffer() const override { return m_UniformBuffer; }

	private:
		Ref<UniformBuffer> m_UniformBuffer;
	};
}
