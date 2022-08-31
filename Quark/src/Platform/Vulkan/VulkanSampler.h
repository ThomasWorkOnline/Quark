#pragma once

#include "Quark/Renderer/Sampler.h"
#include "VulkanDevice.h"

#include <vulkan/vulkan.h>

namespace Quark {

	class VulkanSampler2D final : public Sampler2D
	{
	public:
		VulkanSampler2D(VulkanDevice* device, const Sampler2DSpecification& spec);
		~VulkanSampler2D() final override;

		virtual bool operator==(const Sampler2D& other) const final override
		{
			return m_Sampler == static_cast<const VulkanSampler2D&>(other).m_Sampler;
		}

		// Non-Copyable
		VulkanSampler2D(const VulkanSampler2D&) = delete;
		VulkanSampler2D& operator=(const VulkanSampler2D&) = delete;

		VkSampler GetVkHandle() const { return m_Sampler; }

	private:
		VulkanDevice* m_Device;
		VkSampler m_Sampler;
	};
}
