#pragma once

#include "Quark/Renderer/UniformBuffer.h"

typedef unsigned int GLuint;

namespace Quark {

	class OpenGLUniformBuffer final : public UniformBuffer
	{
	public:
		OpenGLUniformBuffer() = default;
		OpenGLUniformBuffer(const UniformBufferSpecification& spec);
		virtual ~OpenGLUniformBuffer() final override;

		virtual void SetData(const void* data, size_t size, size_t offset = 0) final override;

		virtual bool operator==(const UniformBuffer& other) const final override;

		// Non-Copyable
		OpenGLUniformBuffer(const OpenGLUniformBuffer&) = delete;
		OpenGLUniformBuffer& operator=(const OpenGLUniformBuffer&) = delete;

		GLuint GetRendererID() const { return m_RendererID; }

	private:
		GLuint m_RendererID = 0;
	};
}
