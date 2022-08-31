#pragma once

#include "Quark/Renderer/RenderPass.h"
#include "VulkanDevice.h"

#include <vulkan/vulkan.h>

namespace Quark {

	class VulkanRenderPass final : public RenderPass
	{
	public:
		VulkanRenderPass(VulkanDevice* device, const RenderPassSpecification& spec);
		virtual ~VulkanRenderPass() final override;

		virtual bool operator==(const RenderPass& other) const final override
		{
			return m_RenderPass == reinterpret_cast<const VulkanRenderPass&>(other).m_RenderPass;
		}

		// Non-Copyable
		VulkanRenderPass(const VulkanRenderPass&) = delete;
		VulkanRenderPass& operator=(const VulkanRenderPass&) = delete;

		VkRenderPass GetVkHandle() const { return m_RenderPass; }

	private:
		VulkanDevice* m_Device;
		VkRenderPass m_RenderPass = VK_NULL_HANDLE;
	};
}
