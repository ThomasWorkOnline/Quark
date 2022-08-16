#pragma once

#include "Quark/Renderer/Pipeline.h"
#include "OpenGLUniformBuffer.h"

namespace Quark {

	class OpenGLPipeline final : public Pipeline
	{
	public:
		OpenGLPipeline(const PipelineSpecification& spec);
		virtual ~OpenGLPipeline() override;

		virtual void SetViewport(uint32_t viewportWidth, uint32_t viewportHeight) override {}

		virtual bool operator==(const Pipeline& other) const override
		{
			return true;
		}
	};
}
