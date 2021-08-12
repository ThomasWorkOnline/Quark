#pragma once

#include "../Core/Core.h"

#include "UIElement.h"

namespace Quark {

	class Text : public UIElement
	{
	public:
		Text(const std::string& text);

	private:
		std::string m_Text;
	};
}
