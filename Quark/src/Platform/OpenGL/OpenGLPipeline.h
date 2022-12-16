#pragma once

#include "Quark/Renderer/Pipeline.h"

typedef unsigned int GLuint;
typedef unsigned int GLenum;

namespace Quark {

	class OpenGLPipeline final : public Pipeline
	{
	public:
		OpenGLPipeline(const PipelineSpecification& spec);
		virtual ~OpenGLPipeline() final override;

		virtual bool operator==(const Pipeline& other) const final override;

		// Non-Copyable
		OpenGLPipeline(const OpenGLPipeline&) = delete;
		OpenGLPipeline& operator=(const OpenGLPipeline&) = delete;

		GLenum GetPrimitiveTopologyState() const { return m_PrimitiveTopologyState; }

		void Bind() const;
		void BindVertexAttrib() const;

	private:
		GLuint m_RendererID = 0;

		// Static states
		GLenum m_PrimitiveTopologyState = 0;
		GLenum m_BlendSrcFactorState = 0;
		GLenum m_BlendDstFactorState = 0;
	};
}
