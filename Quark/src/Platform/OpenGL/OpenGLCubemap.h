#pragma once

#include "Quark/Renderer/Cubemap.h"

namespace Quark {

	class OpenGLCubemap final : public Cubemap
	{
	public:
		OpenGLCubemap(const CubemapSpecification& spec);
		virtual ~OpenGLCubemap() override;

		virtual void Attach(uint32_t textureSlot = 0) const override;
		virtual void Detach() const override;

		virtual void SetData(uint32_t index, const void* data, size_t size) override;

		virtual bool operator==(const Cubemap& other) const override
		{
			return m_RendererID == ((OpenGLCubemap&)other).m_RendererID;
		}

	private:
		uint32_t m_RendererID = 0;
	};
}
