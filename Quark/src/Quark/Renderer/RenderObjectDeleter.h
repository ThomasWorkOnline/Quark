#pragma once

#include "../Core/Core.h"
#include "RenderObjectManager.h"

namespace Quark {
	
	class DeferredRenderObjectDeleter
	{
	public:
		template<typename T>
		void operator()(T* object) const noexcept
		{
			RenderObjectManager::DeferredDelete((RenderObject*)object);
		}
	};
}
