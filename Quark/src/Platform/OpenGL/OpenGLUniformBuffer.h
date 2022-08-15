#pragma once

#include "Quark/Renderer/UniformBuffer.h"

typedef unsigned int GLuint;

namespace Quark {

	class OpenGLUniformBuffer final : public UniformBuffer
	{
	public:
		OpenGLUniformBuffer() = default;
		OpenGLUniformBuffer(size_t size, uint32_t binding);
		virtual ~OpenGLUniformBuffer() override;

		virtual void SetData(const void* data, size_t size, size_t offset = 0) override;

		virtual bool operator==(const UniformBuffer& other) const override
		{
			return m_RendererID == reinterpret_cast<decltype(*this)&>(other).m_RendererID;
		}

	private:
		GLuint m_RendererID = 0;
	};
}
