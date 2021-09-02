#pragma once

#include "../Core/Core.h"

#include <atomic>

namespace Quark {

	class RenderObject
	{
	public:
		const std::atomic_bool& Released() const { return m_Released; }

		RenderObject()
		{

		}

		virtual ~RenderObject()
		{
			std::cout << "Render Object destroyed\n";
		}

	private:
		std::atomic_bool m_Released;
	};
}
