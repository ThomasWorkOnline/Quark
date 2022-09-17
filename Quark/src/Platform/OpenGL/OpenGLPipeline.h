#pragma once

#include "Quark/Renderer/Pipeline.h"

#include "OpenGLSampler.h"
#include "OpenGLUniformBuffer.h"

typedef unsigned int GLuint;
typedef unsigned int GLenum;

namespace Quark {

	class OpenGLPipeline final : public Pipeline
	{
	public:
		OpenGLPipeline(const PipelineSpecification& spec);
		virtual ~OpenGLPipeline() final override;

		virtual void SetTexture(const Texture* texture, uint32_t textureIndex) final override;
		virtual void PushDescriptorSets() final override {}

		virtual bool operator==(const Pipeline& other) const final override;

		// Non-Copyable
		OpenGLPipeline(const OpenGLPipeline&) = delete;
		OpenGLPipeline& operator=(const OpenGLPipeline&) = delete;

		GLenum GetPrimitiveTopology() const { return m_PrimitiveTopology; }

		void Bind() const;
		void BindVertexAttrib() const;

	private:
		GLuint m_RendererID = 0;
		GLenum m_PrimitiveTopology = 0;
	};
}
