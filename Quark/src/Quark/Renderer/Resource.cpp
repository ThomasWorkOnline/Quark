#include "Resource.h"

#include "../Core/Application.h"

namespace Quark {

	static std::atomic<int32_t> s_IncrementID = 0;

	Resource::Resource()
		: m_ID(s_IncrementID++)
	{
	}

	void Resource::Hold(const Ref<Resource>& res)
	{
		Application::Get().GetResourceManager().Hold(res);
	}
}
