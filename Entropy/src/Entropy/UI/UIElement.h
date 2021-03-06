#pragma once

#include "../Core/Core.h"

namespace Entropy {

	class UIElement
	{
	public:
		UIElement() = default;
		virtual ~UIElement() = default;

	protected:
		UIElement* m_Parent = nullptr;
	};
}
