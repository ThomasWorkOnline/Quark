#include "RenderObjectManager.h"

namespace Quark {

	std::mutex RenderObjectManager::m_RenderObjectsMutex;
	std::stack<RenderObject*> RenderObjectManager::m_RenderObjects;

	void RenderObjectManager::DeferredDelete(RenderObject* renderObject)
	{
		std::lock_guard<std::mutex> lock(m_RenderObjectsMutex);
		m_RenderObjects.push(renderObject);
	}

	void RenderObjectManager::ReleaseRenderObjects()
	{
		while (HasRenderObjects())
		{
			RenderObject* object = nullptr;
			{
				std::lock_guard<std::mutex> lock(m_RenderObjectsMutex);
				object = m_RenderObjects.top();
				m_RenderObjects.pop();
			}

			delete object;
		}
	}

	bool RenderObjectManager::HasRenderObjects()
	{
		std::lock_guard<std::mutex> lock(m_RenderObjectsMutex);
		return !m_RenderObjects.empty();
	}
}
