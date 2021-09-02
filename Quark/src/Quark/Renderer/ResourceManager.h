#pragma once

#include "../Core/Core.h"

#include <mutex>
#include <unordered_map>

#include "Resource.h"

namespace Quark {

	class ResourceManager
	{
	public:
		void Hold(const Ref<Resource>& resource);

		const Ref<Resource> Get(uint32_t id) const;

		/// <summary>
		/// Must be invoked from the main thread
		/// </summary>
		/// <returns></returns>
		void GarbageCollectResources();

	private:
		mutable std::mutex m_ResourcesMutex;
		std::unordered_map<uint32_t, Ref<Resource>> m_Resources;
	};
}
