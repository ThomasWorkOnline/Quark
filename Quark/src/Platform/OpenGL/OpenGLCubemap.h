#pragma once

#include "Quark/Renderer/Cubemap.h"

typedef unsigned int GLuint;

namespace Quark {

	class OpenGLCubemap final : public Cubemap
	{
	public:
		OpenGLCubemap(const CubemapSpecification& spec);
		virtual ~OpenGLCubemap() final override;

		virtual void Attach(uint32_t textureSlot = 0) const final override;
		virtual void Detach() const final override;

		virtual void SetData(uint32_t index, const void* data, size_t size) final override;

		virtual bool operator==(const Cubemap& other) const final override
		{
			return m_RendererID == reinterpret_cast<decltype(*this)&>(other).m_RendererID;
		}

		// Non-Copyable
		OpenGLCubemap(const OpenGLCubemap&) = delete;
		OpenGLCubemap& operator=(const OpenGLCubemap&) = delete;

	private:
		GLuint m_RendererID;
	};
}
