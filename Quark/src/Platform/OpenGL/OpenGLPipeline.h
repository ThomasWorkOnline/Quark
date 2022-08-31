#pragma once

#include "Quark/Renderer/Pipeline.h"

typedef unsigned int GLenum;

namespace Quark {

	class OpenGLPipeline final : public Pipeline
	{
	public:
		OpenGLPipeline(const PipelineSpecification& spec);
		virtual ~OpenGLPipeline() final override;

		virtual bool operator==(const Pipeline& other) const final override
		{
			return false;
		}

		// Non-Copyable
		OpenGLPipeline(const OpenGLPipeline&) = delete;
		OpenGLPipeline& operator=(const OpenGLPipeline&) = delete;

		void Bind();
		void BindVertexAttrib();

		GLenum GetPrimitiveTopology() const { return m_PrimitiveTopology; }

	private:
		GLenum m_PrimitiveTopology = 0;
	};
}
