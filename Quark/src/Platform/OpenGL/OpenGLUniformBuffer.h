#pragma once

#include "Quark/Renderer/UniformBuffer.h"

namespace Quark {

	class OpenGLUniformBuffer : public UniformBuffer
	{
	public:
		OpenGLUniformBuffer(size_t size, uint32_t binding);
		~OpenGLUniformBuffer() override;

		uint32_t GetRendererID() const override { return m_RendererID; }
		void SetData(const void* data, size_t size, size_t offset = 0) override;

	private:
		uint32_t m_RendererID = 0;
	};
}