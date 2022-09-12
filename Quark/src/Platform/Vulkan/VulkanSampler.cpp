#include "qkpch.h"
#include "VulkanSampler.h"

#include "VulkanEnums.h"

namespace Quark {

	VulkanSampler::VulkanSampler(VulkanDevice* device, const SamplerSpecification& spec)
		: Sampler(spec), m_Device(device)
	{
		VkSamplerAddressMode addressMode = TextureTilingModeToVulkan(m_Spec.RenderModes.TilingMode);

		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType            = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter        = TextureFilteringModeToVulkan(m_Spec.RenderModes.MagFilteringMode);
		samplerInfo.minFilter        = TextureFilteringModeToVulkan(m_Spec.RenderModes.MinFilteringMode);
		samplerInfo.mipmapMode       = TextureMipmapModeToVulkan(m_Spec.RenderModes.MinFilteringMode);
		samplerInfo.addressModeU     = addressMode;
		samplerInfo.addressModeV     = addressMode;
		samplerInfo.addressModeW     = addressMode;
		samplerInfo.mipLodBias       = 0.0f;
		samplerInfo.anisotropyEnable = VK_TRUE;
		samplerInfo.maxAnisotropy    = m_Device->GetPhysicalDeviceProperties().limits.maxSamplerAnisotropy;
		samplerInfo.minLod           = 0.0f;
		samplerInfo.maxLod           = 0.0f;
		samplerInfo.borderColor      = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;

		vkCreateSampler(m_Device->GetVkHandle(), &samplerInfo, nullptr, &m_Sampler);
	}

	VulkanSampler::~VulkanSampler()
	{
		vkDestroySampler(m_Device->GetVkHandle(), m_Sampler, nullptr);
	}

	bool VulkanSampler::operator==(const Sampler& other) const
	{
		if (auto* o = dynamic_cast<decltype(this)>(&other))
			return m_Sampler == o->m_Sampler;

		return false;
	}
}
