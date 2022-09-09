#include "qkpch.h"
#include "VulkanSampler.h"

namespace Quark {

	VulkanSampler2D::VulkanSampler2D(VulkanDevice* device, const Sampler2DSpecification& spec)
		: Sampler2D(spec), m_Device(device)
	{
		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_NEAREST;
		samplerInfo.minFilter = VK_FILTER_NEAREST;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.anisotropyEnable = VK_TRUE;
		samplerInfo.maxAnisotropy = m_Device->GetPhysicalDeviceProperties().limits.maxSamplerAnisotropy;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = 0.0f;
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;

		vkCreateSampler(m_Device->GetVkHandle(), &samplerInfo, nullptr, &m_Sampler);
	}

	VulkanSampler2D::~VulkanSampler2D()
	{
		vkDestroySampler(m_Device->GetVkHandle(), m_Sampler, nullptr);
	}
}
