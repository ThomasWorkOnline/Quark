#pragma once

#include "../Core/Core.h"
#include "../Core/Application.h"

#include "DeferredObject.h"
#include "DeferredObjectManager.h"

namespace Quark {
	
	class DeferredObjectDeleter
	{
	public:
		void operator()(DeferredObject* object) const noexcept
		{
			if (std::this_thread::get_id() == Application::Get().GetThreadId())
			{
				delete object;
			}
			else
			{
				DeferredObjectManager::DeferredDelete(object);
			}
		}
	};
}
