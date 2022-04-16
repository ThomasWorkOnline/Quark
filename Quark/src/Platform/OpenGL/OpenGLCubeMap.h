#pragma once

#include "Quark/Renderer/CubeMap.h"

namespace Quark {

	class OpenGLCubeMap : public CubeMap
	{
	public:
		OpenGLCubeMap(uint32_t width, uint32_t height);
		~OpenGLCubeMap() override;

		void Attach(uint32_t textureSlot = 0) const override;
		void Detach() const override;

		uint32_t GetRendererID() const override { return m_RendererID; }

		bool operator==(const CubeMap& other) const override
		{
			return m_RendererID == ((OpenGLCubeMap&)other).m_RendererID;
		}

	private:
		uint32_t m_RendererID = 0;
		uint32_t m_Width, m_Height;
	};
}
