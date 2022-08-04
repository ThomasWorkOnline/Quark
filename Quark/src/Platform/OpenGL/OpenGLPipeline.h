#pragma once

#include "Quark/Renderer/Pipeline.h"
#include "Quark/Renderer/VertexArray.h"

namespace Quark {

	class OpenGLPipeline final : public Pipeline
	{
	public:
		OpenGLPipeline(const PipelineSpecification& spec);
		virtual ~OpenGLPipeline() override;

		virtual void Resize(uint32_t viewportWidth, uint32_t viewportHeight) override {}

		virtual const Ref<UniformBuffer>& GetUniformBuffer() const override { return m_UniformBuffer; }

		virtual bool operator==(const Pipeline& other) const override
		{
			return true;
		}

	private:
		Ref<UniformBuffer> m_UniformBuffer;
	};
}
