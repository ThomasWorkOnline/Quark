#pragma once

#include "Quark/Renderer/RenderPass.h"

#include "Vulkan.h"
#include "VulkanDevice.h"

namespace Quark {

	class VulkanRenderPass final : public RenderPass
	{
	public:
		VulkanRenderPass(VulkanDevice* device, const RenderPassSpecification& spec);
		virtual ~VulkanRenderPass() final override;

		virtual bool operator==(const RenderPass& other) const final override;

		// Non-Copyable
		VulkanRenderPass(const VulkanRenderPass&) = delete;
		VulkanRenderPass& operator=(const VulkanRenderPass&) = delete;

		VkRenderPass GetVkHandle() const { return m_RenderPass; }

	private:
		VulkanDevice* m_Device;
		VkRenderPass m_RenderPass = nullptr;
	};
}
