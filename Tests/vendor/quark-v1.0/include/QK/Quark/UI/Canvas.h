#pragma once

#include "../Core/Core.h"

#include "UIElement.h"

namespace Quark {

	class Canvas : public UIElement
	{
	public:
		// TODO: framebuffer

	private:
		float m_Width, m_Height;
	};
}
