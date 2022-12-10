#include "qkpch.h"
#include "VulkanRenderPass.h"

#include "VulkanEnums.h"

namespace Quark {

	VulkanRenderPass::VulkanRenderPass(VulkanDevice* device, const RenderPassSpecification& spec)
		: RenderPass(spec)
		, m_Device(device)
	{
		VkPipelineStageFlags srcStageFlags{};
		VkPipelineStageFlags dstStageFlags{};
		VkAccessFlags dstAccessFlags{};

		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = PipelineBindPointToVulkan(m_Spec.BindPoint);

		m_AttachmentCount += m_Spec.ColorAttachmentFormat != ColorFormat::None;
		m_AttachmentCount += m_Spec.Samples > SampleCount::SampleCount1;
		m_AttachmentCount += m_Spec.DepthAttachmentFormat != ColorFormat::None;

		AutoRelease<VkAttachmentDescription> attachments = StackAlloc(m_AttachmentCount * sizeof(VkAttachmentDescription));
		AutoRelease<VkAttachmentReference> attachmentRefs = StackAlloc(m_AttachmentCount * sizeof(VkAttachmentReference));
		uint32_t attachmentIndex = 0;
	
		// Color attachment
		if (m_Spec.ColorAttachmentFormat != ColorFormat::None)
		{
			VkAttachmentDescription& colorAttachment = attachments[attachmentIndex];
			colorAttachment = {};
			colorAttachment.format         = DataFormatToVulkan(m_Spec.ColorAttachmentFormat);
			colorAttachment.samples        = SampleCountToVulkan(m_Spec.Samples);
			colorAttachment.loadOp         = GetVulkanLoadOrClearOp(m_Spec.ClearBuffers);
			colorAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
			colorAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			colorAttachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
			colorAttachment.finalLayout    = m_Spec.Samples > SampleCount::SampleCount1 ? VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

			VkAttachmentReference& colorAttachmentRef = attachmentRefs[attachmentIndex];
			colorAttachmentRef = {};
			colorAttachmentRef.attachment = attachmentIndex;
			colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			attachmentIndex++;

			srcStageFlags |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dstStageFlags |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dstAccessFlags |= VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

			subpass.pColorAttachments = &colorAttachmentRef;
			subpass.colorAttachmentCount++;
		}

		// Resolve attachments
		if (m_Spec.Samples > SampleCount::SampleCount1)
		{
			VkAttachmentDescription& colorAttachmentResolve = attachments[attachmentIndex];
			colorAttachmentResolve = {};
			colorAttachmentResolve.format         = DataFormatToVulkan(m_Spec.ColorAttachmentFormat);
			colorAttachmentResolve.samples        = VK_SAMPLE_COUNT_1_BIT;
			colorAttachmentResolve.loadOp         = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			colorAttachmentResolve.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
			colorAttachmentResolve.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			colorAttachmentResolve.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
			colorAttachmentResolve.finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

			VkAttachmentReference& colorAttachmentResolveRef = attachmentRefs[attachmentIndex];
			colorAttachmentResolveRef = {};
			colorAttachmentResolveRef.attachment = attachmentIndex;
			colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			attachmentIndex++;

			subpass.pResolveAttachments = &colorAttachmentResolveRef;
		}

		// Depth attachment
		if (m_Spec.DepthAttachmentFormat != ColorFormat::None)
		{
			VkAttachmentDescription& depthAttachment = attachments[attachmentIndex];
			depthAttachment = {};
			depthAttachment.format         = DataFormatToVulkan(m_Spec.DepthAttachmentFormat);
			depthAttachment.samples        = SampleCountToVulkan(m_Spec.Samples);
			depthAttachment.loadOp         = GetVulkanLoadOrClearOp(m_Spec.ClearBuffers);
			depthAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			depthAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			depthAttachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
			depthAttachment.finalLayout    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

			VkAttachmentReference& depthAttachmentRef = attachmentRefs[attachmentIndex];
			depthAttachmentRef = {};
			depthAttachmentRef.attachment = attachmentIndex;
			depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			attachmentIndex++;

			srcStageFlags |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
			dstStageFlags |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
			dstAccessFlags |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

			subpass.pDepthStencilAttachment = &depthAttachmentRef;
		}

		VkSubpassDependency dependency{};
		dependency.srcSubpass          = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass          = 0;
		dependency.srcStageMask        = srcStageFlags;
		dependency.srcAccessMask       = VK_ACCESS_NONE;
		dependency.dstStageMask        = dstStageFlags;
		dependency.dstAccessMask       = dstAccessFlags;

		VkRenderPassCreateInfo renderPassInfo{};
		renderPassInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = m_AttachmentCount;
		renderPassInfo.pAttachments    = attachments;
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
