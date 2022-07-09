#include "qkpch.h"
#include "VulkanRenderPass.h"
#include "VulkanContext.h"

namespace Quark {

	namespace Utils {

		VkPipelineBindPoint BindPointToVulkan(PipelineBindPoint bindpoint)
		{
			switch (bindpoint)
			{
				case PipelineBindPoint::Graphics: return VK_PIPELINE_BIND_POINT_GRAPHICS;
				case PipelineBindPoint::Compute:  return VK_PIPELINE_BIND_POINT_COMPUTE;
				default:
					QK_CORE_FATAL("Unknown bindpoint");
					return VkPipelineBindPoint{};
			}
		}
	}

	VulkanRenderPass::VulkanRenderPass(const RenderPassSpecification& spec)
		: m_Spec(spec)
	{
		// TODO: get format from specification
		auto& swapChain = VulkanContext::GetSwapChain();
		VkFormat swapChainImageFormat = (VkFormat)swapChain.m_ActualSurfaceFormat.format;

		VkAttachmentDescription colorAttachment{};
		colorAttachment.format = swapChainImageFormat;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttachmentRef{};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = Utils::BindPointToVulkan(m_Spec.BindPoint);
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;

		VkSubpassDependency dependency{};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = VK_ACCESS_NONE;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		VkRenderPassCreateInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = 1;
		renderPassInfo.pAttachments = &colorAttachment;
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		auto vkDevice = VulkanContext::GetCurrentDevice().GetVkHandle();
		vkCreateRenderPass(vkDevice, &renderPassInfo, nullptr, &m_RenderPass);
	}

	VulkanRenderPass::~VulkanRenderPass()
	{
		auto vkDevice = VulkanContext::GetCurrentDevice().GetVkHandle();
		vkDestroyRenderPass(vkDevice, m_RenderPass, nullptr);
	}
}
