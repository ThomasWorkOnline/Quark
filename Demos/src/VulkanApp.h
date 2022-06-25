#pragma once

#include <Quark.h>
#include <vulkan/vulkan.hpp>

#include "Platform/Vulkan/VulkanFramebuffer.h"
#include "Platform/Vulkan/VulkanRenderPass.h"
#include "Platform/Vulkan/VulkanShader.h"

using namespace Quark;

class VulkanApp : public Application
{
public:
	VulkanApp();
	virtual ~VulkanApp() override;

	virtual void OnEvent(Event& e) override;
	virtual void OnRender() override;

private:
	bool OnWindowResized(WindowResizedEvent& e);
	void RecordCommandBuffer(vk::CommandBuffer commandBuffer, uint32_t imageIndex);

	void RecreateFramebuffers();
	void RecreateGraphicsPipeline();

private:
	Scope<VulkanRenderPass> m_RenderPass;

	VulkanShader m_VertShader;
	VulkanShader m_FragShader;

	vk::PipelineLayout m_VkPipelineLayout;
	vk::Pipeline m_VkGraphicsPipeline;
	vk::CommandPool m_VkCommandPool;

	std::vector<vk::Framebuffer> m_VkSwapChainFramebuffers;
	std::vector<vk::CommandBuffer> m_VkCommandBuffers;
	std::vector<vk::Fence> m_VkInFlightFences;
	uint32_t m_CurrentFrameIndex = 0;
};
