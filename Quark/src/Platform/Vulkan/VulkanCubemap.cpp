#include "qkpch.h"
#include "VulkanCubemap.h"

namespace Quark {

	VulkanCubemap::VulkanCubemap(VulkanDevice* device, const CubemapSpecification& spec)
		: Cubemap(spec)
		, m_Device(device)
	{
	}

	VulkanCubemap::~VulkanCubemap()
	{
	}

	void VulkanCubemap::SetData(const void* data, size_t size, uint32_t index)
	{
	}

	bool VulkanCubemap::operator==(const Texture& other) const
	{
		return false;
	}
}
