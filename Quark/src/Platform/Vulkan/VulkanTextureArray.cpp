#include "qkpch.h"
#include "VulkanTextureArray.h"

namespace Quark {

	VulkanTexture2DArray::VulkanTexture2DArray(VulkanDevice* device, const Texture2DArraySpecification& spec)
		: Texture2DArray(spec)
		, m_Device(device)
	{
	}

	bool VulkanTexture2DArray::operator==(const Texture& other) const
	{
		return false;
	}
}
