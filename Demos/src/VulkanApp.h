#pragma once

#include <Quark.h>
#include <vulkan/vulkan.hpp>

#include "Platform/Vulkan/VulkanRenderPass.h"

using namespace Quark;

class VulkanApp : public Application
{
public:
	VulkanApp();
	virtual ~VulkanApp() override;

	virtual void OnRender() override;

private:
	void RecordCommandBuffer(uint32_t imageIndex);

private:
	Scope<VulkanRenderPass> m_RenderPass;

	vk::PipelineLayout m_VkPipelineLayout;
	vk::Pipeline m_VkGraphicsPipeline;
	vk::Fence m_VkInFlightFence;

	std::vector<vk::Framebuffer> m_VkSwapChainFramebuffers;
};
