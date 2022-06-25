#include "qkpch.h"
#include "VulkanRenderPass.h"
#include "VulkanGraphicsContext.h"

namespace Quark {

	VulkanRenderPass::VulkanRenderPass(vk::Format swapChainImageFormat)
		: m_VkDevice(VulkanGraphicsContext::GetCurrentDevice().GetVkHandle())
	{
		QK_PROFILE_FUNCTION();

		vk::AttachmentDescription colorAttachment;
		colorAttachment.setFormat(swapChainImageFormat);
		colorAttachment.setSamples(vk::SampleCountFlagBits::e1);
		colorAttachment.setLoadOp(vk::AttachmentLoadOp::eClear);
		colorAttachment.setStoreOp(vk::AttachmentStoreOp::eStore);

		colorAttachment.setInitialLayout(vk::ImageLayout::eUndefined);
		colorAttachment.setFinalLayout(vk::ImageLayout::ePresentSrcKHR);

		vk::AttachmentReference colorAttachmentRef;
		colorAttachmentRef.setAttachment(0);
		colorAttachmentRef.setLayout(vk::ImageLayout::eColorAttachmentOptimal);

		vk::SubpassDescription subpass;
		subpass.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics);
		subpass.setColorAttachmentCount(1);
		subpass.setPColorAttachments(&colorAttachmentRef);

		vk::SubpassDependency dependency;
		dependency.setSrcSubpass(VK_SUBPASS_EXTERNAL);
		dependency.setDstSubpass(0);
		dependency.setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
		dependency.setSrcAccessMask(vk::AccessFlagBits::eNone);
		dependency.setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
		dependency.setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite);

		vk::RenderPassCreateInfo renderPassInfo;
		renderPassInfo.setAttachmentCount(1);
		renderPassInfo.setPAttachments(&colorAttachment);
		renderPassInfo.setSubpassCount(1);
		renderPassInfo.setPSubpasses(&subpass);
		renderPassInfo.setDependencyCount(1);
		renderPassInfo.setPDependencies(&dependency);

		m_VkRenderPass = m_VkDevice.createRenderPass(renderPassInfo);
	}

	VulkanRenderPass::~VulkanRenderPass()
	{
		m_VkDevice.destroyRenderPass(m_VkRenderPass);
	}
}
