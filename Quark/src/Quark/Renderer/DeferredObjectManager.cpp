#include "DeferredObjectManager.h"

namespace Quark {

	std::mutex DeferredObjectManager::s_DeferredObjectsMutex;
	std::stack<DeferredObject*> DeferredObjectManager::s_DeferredObjects;

	void DeferredObjectManager::DeferredDelete(DeferredObject* object)
	{
		std::lock_guard<std::mutex> lock(s_DeferredObjectsMutex);
		s_DeferredObjects.push(object);
	}

	void DeferredObjectManager::ReleaseRenderObjects()
	{
		while (HasObjects())
		{
			DeferredObject* object = nullptr;
			{
				std::lock_guard<std::mutex> lock(s_DeferredObjectsMutex);
				object = s_DeferredObjects.top();
				s_DeferredObjects.pop();
			}

			delete object;
		}
	}

	bool DeferredObjectManager::HasObjects()
	{
		std::lock_guard<std::mutex> lock(s_DeferredObjectsMutex);
		return !s_DeferredObjects.empty();
	}
}
