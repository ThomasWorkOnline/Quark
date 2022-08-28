#include "qkpch.h"
#include "VulkanSampler.h"

namespace Quark {

	VulkanSampler2D::VulkanSampler2D(VulkanDevice* device, const Sampler2DSpecification& spec)
		: Sampler2D(spec), m_Device(device)
	{
		VkSamplerCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;

		vkCreateSampler(m_Device->GetVkHandle(), &createInfo, nullptr, &m_Sampler);
	}

	VulkanSampler2D::~VulkanSampler2D()
	{
		vkDestroySampler(m_Device->GetVkHandle(), m_Sampler, nullptr);
	}
}
