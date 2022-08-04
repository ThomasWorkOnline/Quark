#pragma once

#include "Quark/Renderer/RenderPass.h"
#include "VulkanDevice.h"

#include <vulkan/vulkan.h>

namespace Quark {

	class VulkanRenderPass final : public RenderPass
	{
	public:
		VulkanRenderPass(VulkanDevice* device, const RenderPassSpecification& spec);
		virtual ~VulkanRenderPass() override;

		virtual bool operator==(const RenderPass& other) const override
		{
			return m_RenderPass == reinterpret_cast<const VulkanRenderPass&>(other).m_RenderPass;
		}

		VkRenderPass GetVkHandle() const { return m_RenderPass; }

	private:
		VulkanDevice* m_Device;

		VkRenderPass m_RenderPass = VK_NULL_HANDLE;
	};
}
