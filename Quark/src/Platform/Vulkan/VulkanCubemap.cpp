#include "qkpch.h"
#include "VulkanCubemap.h"

namespace Quark {

	VulkanCubemap::VulkanCubemap(VulkanDevice* device, const CubemapSpecification& spec)
		: Cubemap(spec)
		, m_Device(device)
	{
	}
}
