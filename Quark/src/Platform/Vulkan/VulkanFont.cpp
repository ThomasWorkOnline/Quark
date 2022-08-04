#include "qkpch.h"
#include "VulkanFont.h"

namespace Quark {

	VulkanFont::VulkanFont(VulkanDevice* device, std::string_view filepath, uint32_t fontSize)
		: m_Device(device)
	{
	}
}
