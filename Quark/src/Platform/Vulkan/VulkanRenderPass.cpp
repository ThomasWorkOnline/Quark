#include "qkpch.h"
#include "VulkanRenderPass.h"

#include "VulkanFormats.h"

namespace Quark {

	namespace Utils {

		VkPipelineBindPoint BindPointToVulkan(PipelineBindPoint bindpoint)
		{
			switch (bindpoint)
			{
				case PipelineBindPoint::Graphics: return VK_PIPELINE_BIND_POINT_GRAPHICS;
				case PipelineBindPoint::Compute:  return VK_PIPELINE_BIND_POINT_COMPUTE;

				QK_ASSERT_NO_DEFAULT("Unknown bindpoint");
			}

			return VkPipelineBindPoint{};
		}
	}

	VulkanRenderPass::VulkanRenderPass(VulkanDevice* device, const RenderPassSpecification& spec)
		: RenderPass(spec)
		, m_Device(device)
	{
		VkAttachmentDescription colorAttachment{};
		colorAttachment.format = DataFormatToVulkan(spec.ColorFormat);
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

		vkCreateRenderPass(m_Device->GetVkHandle(), &renderPassInfo, nullptr, &m_RenderPass);
	}

	VulkanRenderPass::~VulkanRenderPass()
	{
		vkDestroyRenderPass(m_Device->GetVkHandle(), m_RenderPass, nullptr);
	}

	bool VulkanRenderPass::operator==(const RenderPass& other) const
	{
		if (auto* o = dynamic_cast<decltype(this)>(&other))
			return m_RenderPass == o->m_RenderPass;

		return false;
	}
}
