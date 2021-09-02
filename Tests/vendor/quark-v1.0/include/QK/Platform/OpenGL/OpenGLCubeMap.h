#pragma once

#include "../../Quark/Renderer/CubeMap.h"

namespace Quark {

	class OpenGLCubeMap : public CubeMap
	{
	public:
		OpenGLCubeMap(const std::array<std::string, 6>& filepaths);
		virtual ~OpenGLCubeMap();

		virtual void Attach(uint32_t textureSlot = 0) const override;
		virtual void Detach() const override;

		virtual uint32_t GetRendererID() const override { return m_RendererID; }

		virtual bool operator==(const Resource& other) const override
		{
			return m_RendererID == ((OpenGLCubeMap&)other).m_RendererID;
		}

	private:
		uint32_t m_RendererID = 0;
		std::array<std::string, 6> m_Filepaths;
		uint32_t m_InternalFormats[6] = {}, m_DataFormats[6] = {};
	};
}
