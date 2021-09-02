#include "Resource.h"

namespace Quark {

	static std::atomic<int32_t> s_IncrementID = 0;

	Resource::Resource()
		: m_ID(s_IncrementID++)
	{
	}
}
