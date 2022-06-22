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
	void RecordCommandBuffer(vk::CommandBuffer commandBuffer, uint32_t imageIndex);

private:
	Scope<VulkanRenderPass> m_RenderPass;

	vk::PipelineLayout m_VkPipelineLayout;
	vk::Pipeline m_VkGraphicsPipeline;
	vk::CommandPool m_VkCommandPool;

	std::vector<vk::CommandBuffer> m_VkCommandBuffers;
	std::vector<vk::Framebuffer> m_VkSwapChainFramebuffers;
	std::vector<vk::Fence> m_VkInFlightFences;
	uint32_t m_CurrentFrameIndex = 0;
};
