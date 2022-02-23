#pragma once

#include "../Core/Core.h"
#include "DeferredObject.h"

#include <mutex>
#include <stack>

namespace Quark {

	class DeferredObjectManager
	{
	public:
		static void DeferredDelete(DeferredObject* object);
		static void ReleaseRenderObjects();

	private:
		static bool HasObjects();

	private:
		static std::mutex s_DeferredObjectsMutex;
		static std::stack<DeferredObject*> s_DeferredObjects;
	};
}
