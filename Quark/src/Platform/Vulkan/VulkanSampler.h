#pragma once

#include "Quark/Renderer/Sampler.h"
#include "VulkanDevice.h"

#include <vulkan/vulkan.h>

namespace Quark {

	class VulkanSampler final : public Sampler
	{
	public:
		VulkanSampler(VulkanDevice* device, const SamplerSpecification& spec);
		~VulkanSampler() final override;

		virtual bool operator==(const Sampler& other) const final override;

		// Non-Copyable
		VulkanSampler(const VulkanSampler&) = delete;
		VulkanSampler& operator=(const VulkanSampler&) = delete;

		VkSampler GetVkHandle() const { return m_Sampler; }

	private:
		VulkanDevice* m_Device;
		VkSampler m_Sampler;
	};
}
