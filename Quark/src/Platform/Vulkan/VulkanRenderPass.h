#pragma once

#include "Quark/Renderer/RenderPass.h"
#include <vulkan/vulkan.hpp>

namespace Quark {

	class VulkanRenderPass final : public RenderPass
	{
	public:
		VulkanRenderPass(const RenderPassSpecification& spec);
		virtual ~VulkanRenderPass() override;

		virtual bool operator==(const RenderPass& other) const override
		{
			return m_RenderPass == reinterpret_cast<const VulkanRenderPass&>(other).m_RenderPass;
		}

		vk::RenderPass GetVkHandle() const { return m_RenderPass; }

	private:
		VkRenderPass m_RenderPass = VK_NULL_HANDLE;
	};
}
