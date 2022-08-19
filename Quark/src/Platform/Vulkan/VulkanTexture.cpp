#include "qkpch.h"
#include "VulkanTexture.h"

namespace Quark {

	VulkanTexture2D::VulkanTexture2D(VulkanDevice* device, const Texture2DSpecification& spec)
		: Texture2D(spec)
		, m_Device(device)
	{
	}

	VulkanTexture2D::VulkanTexture2D(VulkanDevice* device, std::string_view filepath, const TextureRenderModes& renderModes)
		: m_Device(device)
	{
	}
}
