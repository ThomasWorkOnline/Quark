#pragma once

#include "Quark/Renderer/Pipeline.h"

typedef unsigned int GLenum;
#define GL_NONE 0

namespace Quark {

	class OpenGLPipeline final : public Pipeline
	{
	public:
		OpenGLPipeline(const PipelineSpecification& spec);
		virtual ~OpenGLPipeline() override;

		virtual void SetViewport(uint32_t viewportWidth, uint32_t viewportHeight) override {}

		virtual bool operator==(const Pipeline& other) const override
		{
			return false;
		}

		// Non-Copyable
		OpenGLPipeline(const OpenGLPipeline&) = delete;
		OpenGLPipeline& operator=(const OpenGLPipeline&) = delete;

		GLenum GetPrimitiveTopologyState() const { return m_PrimitiveTopology; }

	private:
		GLenum m_PrimitiveTopology = GL_NONE;
	};
}
