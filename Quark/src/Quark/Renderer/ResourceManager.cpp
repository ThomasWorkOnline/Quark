#include "ResourceManager.h"

namespace Quark {

	void ResourceManager::Hold(const Ref<Resource>& resource)
	{
		std::lock_guard<std::mutex> lock(m_ResourcesMutex);
		m_Resources[resource->GetID()] = resource;
	}

	const Ref<Resource> ResourceManager::Get(uint32_t id) const
	{
		std::lock_guard<std::mutex> lock(m_ResourcesMutex);
		auto it = m_Resources.find(id);
		if (it != m_Resources.end())
			return it->second;

		QK_FATAL("Tried to get an non existant resource.");
		return nullptr;
	}

	void ResourceManager::GarbageCollectResources()
	{
		std::lock_guard<std::mutex> lock(m_ResourcesMutex);
		for (auto& res : m_Resources)
		{
			if (res.second.use_count() == 1)
				m_Resources.erase(res.second->GetID());
		}
	}
}
