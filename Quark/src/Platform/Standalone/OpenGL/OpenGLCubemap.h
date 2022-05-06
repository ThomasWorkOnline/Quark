#pragma once

#include "Quark/Renderer/Cubemap.h"

namespace Quark {

	class OpenGLCubemap : public Cubemap
	{
	public:
		OpenGLCubemap(const CubemapSpecification& spec);
		~OpenGLCubemap() override;

		void Attach(uint32_t textureSlot = 0) const override;
		void Detach() const override;

		void SetData(uint32_t index, const void* data, size_t size) override;

		uint32_t GetRendererID() const override { return m_RendererID; }

		bool operator==(const Cubemap& other) const override
		{
			return m_RendererID == ((OpenGLCubemap&)other).m_RendererID;
		}

	private:
		uint32_t m_RendererID = 0;
		CubemapSpecification m_Spec;
	};
}
