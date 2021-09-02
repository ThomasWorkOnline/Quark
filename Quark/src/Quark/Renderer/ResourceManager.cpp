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

	bool ResourceManager::HasGarbage() const
	{
		std::lock_guard<std::mutex> lock(m_ResourcesMutex);
		return !m_Resources.empty();
	}

	void ResourceManager::GarbageCollectResources()
	{
		while (HasGarbage())
		{
			auto resource = GetFirstResource();
			EraseResource(resource->GetID());

			if (resource.use_count() == 1)
				resource.reset();
		}
	}

	void ResourceManager::EraseResource(uint32_t id)
	{
		std::lock_guard<std::mutex> lock(m_ResourcesMutex);
		m_Resources.erase(id);
	}

	const Ref<Resource>& ResourceManager::GetFirstResource()
	{
		std::lock_guard<std::mutex> lock(m_ResourcesMutex);
		return (*m_Resources.begin()).second;
	}
}
