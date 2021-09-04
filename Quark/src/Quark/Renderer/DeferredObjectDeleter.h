#pragma once

#include "../Core/Core.h"
#include "DeferredObjectManager.h"
#include "DeferredObject.h"

namespace Quark {
	
	class DeferredObjectDeleter
	{
	public:
		template<typename T>
		void operator()(T* object) const noexcept
		{
			DeferredObjectManager::DeferredDelete(object);
		}
	};
}
