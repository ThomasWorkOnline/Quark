#pragma once

#include "../../Entropy/Renderer/EnvironmentMap.h"

#include <GL/glew.h>

namespace Entropy {

	class OpenGLEnvironmentMap : public EnvironmentMap
	{
	public:
		OpenGLEnvironmentMap(const std::array<std::string, 6>& filepaths);
		virtual ~OpenGLEnvironmentMap();

		virtual void Attach(uint32_t textureSlot = 0) const override;
		virtual void Detach() const override;

	private:
		uint32_t m_RendererID = 0;
		std::array<std::string, 6> m_Filepaths;
		GLenum m_InternalFormats[6], m_DataFormats[6];
	};
}
