#include "qkpch.h"
#include "VulkanRenderPass.h"

#include "VulkanEnums.h"

namespace Quark {

	VulkanRenderPass::VulkanRenderPass(VulkanDevice* device, const RenderPassSpecification& spec)
		: RenderPass(spec)
		, m_Device(device)
	{
		VkAttachmentDescription colorAttachment{};
		colorAttachment.format         = DataFormatToVulkan(m_Spec.ColorFormat);
		colorAttachment.samples        = SampleCountToVulkan(m_Spec.Samples);
		colorAttachment.loadOp         = GetVulkanLoadOrClearOp(m_Spec.ClearBuffers);
		colorAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkPipelineStageFlags srcStageFlags{};
		VkPipelineStageFlags dstStageFlags{};
		VkAccessFlags dstAccessFlags{};

		VkAttachmentReference colorAttachmentRef{};
		colorAttachmentRef.attachment  = 0;
		colorAttachmentRef.layout      = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		srcStageFlags |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dstStageFlags |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dstAccessFlags |= VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

#if 0
		VkAttachmentReference colorAttachmentResolveRef{};
		colorAttachmentResolveRef.attachment  = 1;
		colorAttachmentResolveRef.layout      = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depthAttachmentRef{};
		depthAttachmentRef.attachment   = 2;
		depthAttachmentRef.layout       = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;

		srcStageFlags |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dstStageFlags |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dstAccessFlags |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
#endif

		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint       = PipelineBindPointToVulkan(m_Spec.BindPoint);
		subpass.colorAttachmentCount    = 1;
		subpass.pColorAttachments       = &colorAttachmentRef;
		//subpass.pResolveAttachments     = &colorAttachmentResolveRef;
		//subpass.pDepthStencilAttachment = &depthAttachmentRef;

		VkSubpassDependency dependency{};
		dependency.srcSubpass          = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass          = 0;
		dependency.srcStageMask        = srcStageFlags;
		dependency.srcAccessMask       = VK_ACCESS_NONE;
		dependency.dstStageMask        = dstStageFlags;
		dependency.dstAccessMask       = dstAccessFlags;

		VkRenderPassCreateInfo renderPassInfo{};
		renderPassInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = 1;
		renderPassInfo.pAttachments    = &colorAttachment;
		renderPassInfo.subpassCount    = 1;
		renderPassInfo.pSubpasses      = &subpass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies   = &dependency;

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
