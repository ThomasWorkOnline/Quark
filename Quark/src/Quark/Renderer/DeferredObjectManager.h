#pragma once

#include "../Core/Core.h"
#include "DeferredObject.h"

#include <mutex>
#include <stack>

namespace Quark {

	class DeferredObjectManager
	{
	public:
		template<typename T>
		static void DeferredDelete(T* object)
		{
			QK_ASSERT(dynamic_cast<DeferredObject*>(object), "Object is not of polymorphic DeferredObject type.");

			std::lock_guard<std::mutex> lock(s_DeferredObjectsMutex);
			s_DeferredObjects.push(object);
		}

		static void ReleaseRenderObjects();

	private:
		static bool HasObjects();

	private:
		static std::mutex s_DeferredObjectsMutex;
		static std::stack<DeferredObject*> s_DeferredObjects;
	};
}
