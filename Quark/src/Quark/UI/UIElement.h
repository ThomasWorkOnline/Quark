#pragma once

#include "Quark/Core/Core.h"

namespace Quark {

	class UIElement
	{
	public:
		UIElement() = default;
		virtual ~UIElement() = default;

		const UIElement& GetParent() const { return *m_Parent; }

	protected:
		UIElement* m_Parent = nullptr;
	};
}
