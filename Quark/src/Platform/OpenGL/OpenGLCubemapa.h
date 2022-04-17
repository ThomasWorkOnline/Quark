#pragma once

#include "Quark/Renderer/Cubemap.h"

namespace Quark {

	class OpenGLCubemap : public Cubemap
	{
	public:
		OpenGLCubemap(uint32_t width, uint32_t height);
		~OpenGLCubemap() override;

		void Attach(uint32_t textureSlot = 0) const override;
		void Detach() const override;

		uint32_t GetRendererID() const override { return m_RendererID; }

		bool operator==(const Cubemap& other) const override
		{
			return m_RendererID == ((OpenGLCubemap&)other).m_RendererID;
		}

	private:
		uint32_t m_RendererID = 0;
		uint32_t m_Width, m_Height;
	};
}
