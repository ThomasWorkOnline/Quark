#pragma once

#include "Quark/Renderer/UniformBuffer.h"

typedef unsigned int GLuint;

namespace Quark {

	class OpenGLUniformBuffer final : public UniformBuffer
	{
	public:
		OpenGLUniformBuffer(size_t size, uint32_t binding);
		virtual ~OpenGLUniformBuffer() override;

		virtual void Attach(uint32_t binding = 0) const override;
		virtual void Detach() const override;

		virtual void SetData(const void* data, size_t size, size_t offset = 0) override;

		virtual bool operator==(const UniformBuffer& other) const override
		{
			return m_RendererID == ((OpenGLUniformBuffer&)other).m_RendererID;
		}

	private:
		GLuint m_RendererID = 0;
	};
}
