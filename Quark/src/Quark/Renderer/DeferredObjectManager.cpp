#include "DeferredObjectManager.h"

namespace Quark {

	std::mutex DeferredObjectManager::m_DeferredObjectsMutex;
	std::stack<DeferredObject*> DeferredObjectManager::m_DeferredObjects;

	void DeferredObjectManager::ReleaseRenderObjects()
	{
		while (HasRenderObjects())
		{
			DeferredObject* object = nullptr;
			{
				std::lock_guard<std::mutex> lock(m_DeferredObjectsMutex);
				object = m_DeferredObjects.top();
				m_DeferredObjects.pop();
			}

			delete object;
		}
	}

	bool DeferredObjectManager::HasRenderObjects()
	{
		std::lock_guard<std::mutex> lock(m_DeferredObjectsMutex);
		return !m_DeferredObjects.empty();
	}
}
