#include "ResourceManager.h"

#include <queue>

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

	static std::queue<uint32_t> s_Released;
	void ResourceManager::GarbageCollectResources()
	{
		{
			std::lock_guard<std::mutex> lock(m_ResourcesMutex);
			for (auto& resource : m_Resources)
			{
				if (resource.second.use_count() == 1)
				{
					s_Released.push(resource.second->GetID());
				}
			}
		}

		while (!s_Released.empty())
		{
			uint32_t id = s_Released.front();
			s_Released.pop();

			std::lock_guard<std::mutex> lock(m_ResourcesMutex);
			m_Resources.erase(id);
		}
	}
}
