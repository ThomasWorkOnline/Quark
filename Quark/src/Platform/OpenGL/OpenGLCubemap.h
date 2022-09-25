#pragma once

#include "Quark/Renderer/Cubemap.h"
#include "OpenGLTexture.h"

namespace Quark {

	class OpenGLCubemap final : public OpenGLTexture, public Cubemap
	{
	public:
		OpenGLCubemap(const CubemapSpecification& spec);
		virtual ~OpenGLCubemap() final override;

		virtual void SetData(uint32_t index, const void* data, size_t size) final override;

		virtual const void* GetHandle() const final override { return static_cast<const OpenGLTexture*>(this); }

		virtual bool operator==(const Texture& other) const final override;

		// Non-Copyable
		OpenGLCubemap(const OpenGLCubemap&) = delete;
		OpenGLCubemap& operator=(const OpenGLCubemap&) = delete;
	};
}
