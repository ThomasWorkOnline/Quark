#pragma once

#include "Quark/Renderer/Sampler.h"
#include "VulkanDevice.h"

#include <vulkan/vulkan.h>

namespace Quark {

	class VulkanSampler2D : public Sampler2D
	{
	public:
		VulkanSampler2D(VulkanDevice* device, const Sampler2DSpecification& spec);
		~VulkanSampler2D();

		// Non-Copyable
		VulkanSampler2D(const VulkanSampler2D&) = delete;
		VulkanSampler2D& operator=(const VulkanSampler2D&) = delete;

	private:
		VulkanDevice* m_Device;
		VkSampler m_Sampler;
	};
}
