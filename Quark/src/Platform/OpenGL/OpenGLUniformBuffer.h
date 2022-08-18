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

		// Non-Copyable
		OpenGLUniformBuffer(const OpenGLUniformBuffer&) = delete;
		OpenGLUniformBuffer& operator=(const OpenGLUniformBuffer&) = delete;

	private:
		GLuint m_RendererID = 0;
		uint32_t m_Binding = 0;
	};
}
